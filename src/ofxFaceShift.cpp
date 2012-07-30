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

ofxFaceShift::ofxFaceShift()
	:found(false) {
}

void ofxFaceShift::setup(unsigned int port) {	
	udpConnection.Create();
	udpConnection.Bind(port);
	udpConnection.SetNonBlocking(true);
	
	blendshapeNames = ofSplitString(ofBufferFromFile("blendshapes.txt"), "\n");
}

// written against http://www.faceshift.com/help/studio/beta/#faceshiftstudiobeta-faceshiftOpenFormat
bool ofxFaceShift::update() {
	bool newFrame = false;
	static char message[maxPacketSize];
	int messageLength = udpConnection.Receive(message, maxPacketSize);
	if(messageLength > 0) {
		newFrame = true;
		
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
					readRaw(data, leftEye.x);
					readRaw(data, leftEye.y);
					readRaw(data, rightEye.x);
					readRaw(data, rightEye.y);
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

ofVec2f ofxFaceShift::getLeftEye() const {
	return leftEye;
}

ofVec2f ofxFaceShift::getRightEye() const {
	return rightEye;
}
