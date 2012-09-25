#include "ofxFaceShift.h"

const unsigned short referenceVersionNumber = 1;
const unsigned int maxPacketSize = 1024;

enum {
	FS_FRAME_INFO_BLOCK = 101,
	FS_POSE_BLOCK = 102,
	FS_BLENDSHAPES_BLOCK = 103,
	FS_EYES_BLOCK = 104,
	FS_MARKERS_BLOCK = 105
};

void checkVersion(unsigned short versionNumber) {
	static bool versionChecked = false;
	if(!versionChecked && versionNumber != referenceVersionNumber) {
		ofLogWarning() << "FaceShift Studio binary protocol is version " << versionNumber << " but ofxFaceShift uses version " << referenceVersionNumber << ". There may be an incompatibility." << endl;
	}
	versionChecked = true;
}

template <class T>
void readRaw(stringstream& stream, T& data) {
	stream.read((char*) &data, sizeof(T));
}

ofVec3f getNormal(const ofVec3f& v1, const ofVec3f& v2, const ofVec3f& v3) {
	ofVec3f a = v1 - v2;
	ofVec3f b = v3 - v2;
	ofVec3f normal = b.cross(a);
	normal.normalize();
	return normal;
}

void buildNormalsAverage(ofMesh& mesh) {
	vector<ofIndexType>& indices = mesh.getIndices();
	vector<ofVec3f> normals(mesh.getNumVertices());
	for(int i = 0; i < indices.size(); i += 3) {
		int i0 = indices[i + 0], i1 = indices[i + 1], i2 = indices[i + 2];
		ofVec3f normal = getNormal(mesh.getVertices()[i0], mesh.getVertices()[i1], mesh.getVertices()[i2]);
		normals[i0] += normal;
		normals[i1] += normal;
		normals[i2] += normal;
	}
	for(int i = 0; i < normals.size(); i++) {
		normals[i].normalize();
	}
	mesh.addNormals(normals);
}

void buildNormalsFaces(ofMesh& mesh) {
	mesh.clearNormals();
	for(int i = 0; i < mesh.getNumVertices(); i += 3) {
		int i0 = i + 0, i1 = i + 1, i2 = i + 2;
		ofVec3f normal = getNormal(mesh.getVertices()[i0], mesh.getVertices()[i1], mesh.getVertices()[i2]);
		for(int j = 0; j < 3; j++) {
			mesh.addNormal(normal);
		}
	}
}

void buildNormals(ofMesh& mesh) {
	if(mesh.getNumIndices() > 0) {
		buildNormalsAverage(mesh);
	} else {
		buildNormalsFaces(mesh);
	}
}

// load an obj file exported from face shift. smoothing drops the texCoords
// for now because they are laid on in a different order than the vertices.
ofMesh loadObj(string filename, bool smooth = false) {
	ofMesh m;
	vector<ofVec3f> v;
	vector<ofVec2f> vt;
	ofFile f(filename);
	while(!f.eof()) {
		string c;
		f >> c;
		if(c.size()) {
			if(c == "v") {
				float x, y, z;
				f >> x >> y >> z;
				if(smooth) {
					m.addVertex(ofVec3f(x, y, z));
				} else {
					v.push_back(ofVec3f(x, y, z));
				}
			} else if(c == "vt") {
				float u, v;
				f >> u >> v;
				if(!smooth) {
					vt.push_back(ofVec2f(u, v));
				}
			} else if(c == "f") {
				string l;
				getline(f, l);
				replace(l.begin(), l.end(), '/', ' ');
				istringstream ls(l);
				int vi1, vti1, vi2, vti2, vi3, vti3;
				ls >> vi1 >> vti1 >> vi2 >> vti2 >> vi3 >> vti3;
				if(smooth) {
					m.addIndex(vi1-1);
					m.addIndex(vi2-1);
					m.addIndex(vi3-1);
				} else {
					m.addVertex(v[vi1-1]);
					m.addVertex(v[vi2-1]);
					m.addVertex(v[vi3-1]);
					m.addTexCoord(vt[vti1-1]);
					m.addTexCoord(vt[vti2-1]);
					m.addTexCoord(vt[vti3-1]);
				}
				if(ls.peek() == ' ') {
					int vi4, vti4;
					ls >> vi4 >> vti4;
					if(smooth) {
						m.addIndex(vi1-1);
						m.addIndex(vi3-1);
						m.addIndex(vi4-1);
					} else {
						m.addVertex(v[vi1-1]); 
						m.addVertex(v[vi3-1]);
						m.addVertex(v[vi4-1]);
						m.addTexCoord(vt[vti1-1]);
						m.addTexCoord(vt[vti2-1]);
						m.addTexCoord(vt[vti3-1]);
					}
				}
			}
		}
	}
	return m;
}

void loadEye(string filename, ofVec3f& leftEyeOffset, ofVec3f& rightEyeOffset) {
	ofFile f(filename);
	f >> leftEyeOffset.x >> leftEyeOffset.y >> leftEyeOffset.z;
	f >> rightEyeOffset.x >> rightEyeOffset.y >> rightEyeOffset.z;
}

ofxFaceShift::ofxFaceShift()
	:found(false)
	,imported(false)
	,blendNeedsUpdating(false) {
}

void ofxFaceShift::setup(unsigned int port) {	
	udpConnection.Create();
	udpConnection.Bind(port);
	udpConnection.SetNonBlocking(true);
	
	blendshapeNames = ofSplitString(ofBufferFromFile("blendshapes.txt"), "\n");
}

void ofxFaceShift::import(string modelFolder) {
	loadEye(modelFolder + "/eye", leftEyeOffset, rightEyeOffset);
	
	ofDirectory dir(modelFolder);
	dir.allowExt("obj");
	dir.listDir();
	blendshapeMeshes.clear();
	for(int i = 0; i < dir.size(); i++) {
		ofLog() << "Loading " << dir.getPath(i);
		ofMesh mesh = loadObj(dir.getPath(i), true);
		buildNormals(mesh);
		if(dir.getName(i) == "Neutral.obj") {
			neutralMesh = mesh;
		} else {
			blendshapeMeshes.push_back(mesh);
		}
	}
	
	validBlendPoints.clear();
	for(int i = 0; i < blendshapeMeshes.size(); i++) {
		validBlendPoints.push_back(vector<unsigned int>());
		for(int j = 0; j < blendshapeMeshes[i].getNumVertices(); j++) {
			blendshapeMeshes[i].getVertices()[j] -= neutralMesh.getVertices()[j];
			if(blendshapeMeshes[i].getVertices()[j] != ofVec3f()) {
				validBlendPoints[i].push_back(j);
			}
		}
	}
	
	imported = true;
}

// written against http://www.faceshift.com/help/studio/beta/#faceshiftstudiobeta-faceshiftOpenFormat
bool ofxFaceShift::update() {
	bool newFrame = false;
	static char message[maxPacketSize];
	int messageLength = udpConnection.Receive(message, maxPacketSize);
	if(messageLength > 0) {
		newFrame = true;
		blendNeedsUpdating = true;
		
		stringstream data;
		data.write(message, messageLength);
		
		unsigned short blockID;
		unsigned int blockSize;
		
		readRaw(data, blockID);
		readRaw(data, versionNumber);
		readRaw(data, blockSize);
		
		checkVersion(versionNumber);
		unsigned short numberBlocks;
		readRaw(data, numberBlocks);
		
		for(int i = 0; i < numberBlocks; i++) {
			readRaw(data, blockID);
			readRaw(data, versionNumber);
			readRaw(data, blockSize);
			
			switch(blockID) {
				case FS_FRAME_INFO_BLOCK:
					readRaw(data, timestamp);
					readRaw(data, found);
					break;
				case FS_POSE_BLOCK:
					readRaw(data, rotation.x());
					readRaw(data, rotation.y());
					readRaw(data, rotation.z());
					readRaw(data, rotation.w());
					readRaw(data, position.x);
					readRaw(data, position.y);
					readRaw(data, position.z);
					break;
				case FS_BLENDSHAPES_BLOCK:
					blendshapeWeights.clear();
					unsigned int blendshapeCount;
					readRaw(data, blendshapeCount);
					for(int i = 0; i < blendshapeCount; i++) {
						float blendshapeWeight;
						readRaw(data, blendshapeWeight);
						blendshapeWeights.push_back(blendshapeWeight);
					}
					break;
				case FS_EYES_BLOCK:
					readRaw(data, leftEyeRotation.y);
					readRaw(data, leftEyeRotation.x);
					readRaw(data, rightEyeRotation.y);
					readRaw(data, rightEyeRotation.x);
					break;
				case FS_MARKERS_BLOCK:
					markers.clear();
					unsigned short markerCount;
					readRaw(data, markerCount);
					for(int i = 0; i < markerCount; i++) {
						ofVec3f marker;
						readRaw(data, marker.x);
						readRaw(data, marker.y);
						readRaw(data, marker.z);
						markers.push_back(marker);
					}
					break;
			}
		}
	}
	return newFrame;
}

unsigned int ofxFaceShift::getBlendshapeCount() const {
	return blendshapeWeights.size();
}

float ofxFaceShift::getBlendshapeWeight(unsigned int i) const {
	return blendshapeWeights.at(i);
}

float ofxFaceShift::getBlendshapeWeight(const string& blendshapeName) const {
	for(int i = 0; i < blendshapeNames.size() && i < blendshapeWeights.size(); i++) {
		if(blendshapeNames[i] == blendshapeName) {
			return blendshapeWeights[i];
		}
	}
	return 0;
}

const vector<float>& ofxFaceShift::getBlendshapeWeights() const {
	return blendshapeWeights;
}

string ofxFaceShift::getBlendshapeName(unsigned int i) const {
	return blendshapeNames.at(i);
}

const vector<string>& ofxFaceShift::getBlendshapeNames() const {
	return blendshapeNames;
}

unsigned int ofxFaceShift::getMarkerCount() const {
	return markers.size();
}

ofVec3f ofxFaceShift::getMarker(unsigned int i) const {
	return markers.at(i);
}

const vector<ofVec3f>& ofxFaceShift::getMarkers() const {
	return markers;
}

unsigned short ofxFaceShift::getVersionNumber() const {
	return versionNumber;
}

double ofxFaceShift::getTimestamp() const {
	return timestamp;
}

bool ofxFaceShift::getFound() const {
	return found;
}

ofQuaternion ofxFaceShift::getRotation() const {
	return rotation;
}

ofVec3f ofxFaceShift::getRotationEuler() const {
	return rotation.getEuler();
}

ofMatrix4x4 ofxFaceShift::getRotationMatrix() const {
	ofMatrix4x4 mat;
	rotation.get(mat);
	return mat;
}

ofVec3f ofxFaceShift::getPosition() const {
	return position;
}

ofVec2f ofxFaceShift::getLeftEyeRotation() const {
	return leftEyeRotation;
}

ofVec2f ofxFaceShift::getRightEyeRotation() const {
	return rightEyeRotation;
}

ofVec3f ofxFaceShift::getLeftEyeOffset() const {
	return leftEyeOffset;
}

ofVec3f ofxFaceShift::getRightEyeOffset() const {
	return rightEyeOffset;
}

ofMesh& ofxFaceShift::getBlendMesh() {
	if(imported && blendNeedsUpdating) {
		blendMesh = neutralMesh;
		for(int i = 0; i < blendshapeMeshes.size(); i++) {		
			float weight = getBlendshapeWeight(i);
			if(weight > 0) {
				for(int j = 0; j < validBlendPoints[i].size(); j++) {
					int k = validBlendPoints[i][j];
					blendMesh.getVertices()[k] += weight * blendshapeMeshes[i].getVertices()[k];
				}
			}
		}
		buildNormals(blendMesh);
	}
	return blendMesh;
}

ofMesh& ofxFaceShift::getBlendshapeMesh(unsigned int i) {
	return blendshapeMeshes.at(i);
}
