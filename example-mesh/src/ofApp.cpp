#include "ofApp.h"

ofVec3f getNormal(const ofVec3f& v1, const ofVec3f& v2, const ofVec3f& v3) {
	ofVec3f a = v1 - v2;
	ofVec3f b = v3 - v2;
	ofVec3f normal = b.cross(a);
	normal.normalize();
	return normal;
}

void buildNormals(ofMesh& mesh) {
	vector<ofVec3f>& vertices = mesh.getVertices();
	mesh.clearNormals();
	for(int i = 0; i < mesh.getNumVertices(); i += 3) {
		ofVec3f normal = getNormal(mesh.getVertices()[i+0], mesh.getVertices()[i+1], mesh.getVertices()[i+2]);
		for(int j = 0; j < 3; j++) {
			mesh.addNormal(normal);
		}
	}
}

ofMesh loadObj(string filename) {
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
				v.push_back(ofVec3f(x, y, z));
			} else if(c == "vt") {
				float u, v;
				f >> u >> v;
				vt.push_back(ofVec2f(u, v));
			} else if(c == "f") {
				string l;
				getline(f, l);
				replace(l.begin(), l.end(), '/', ' ');
				istringstream ls(l);
				int vi1, vti1, vi2, vti2, vi3, vti3;
				ls >> vi1 >> vti1 >> vi2 >> vti2 >> vi3 >> vti3;
				m.addVertex(v[vi1-1]);
				m.addVertex(v[vi2-1]);
				m.addVertex(v[vi3-1]);
				m.addTexCoord(vt[vti1-1]);
				m.addTexCoord(vt[vti2-1]);
				m.addTexCoord(vt[vti3-1]);
				if(ls.peek() == ' ') {
					int vi4, vti4;
					ls >> vi4 >> vti4;
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
	return m;
}

void ofApp::setup() {
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetVerticalSync(true);
	faceShift.setup();
	
	gui.setup();
	const vector<string>& names = faceShift.getBlendshapeNames();
	for(int i = 0; i < names.size(); i++) {
		gui.add(Slider(names[i], 0, 1, 0));
	}
	
	ofDirectory dir("export");
	dir.allowExt("obj");
	dir.listDir();
	for(int i = 0; i < dir.size(); i++) {
		cout << "loading " << dir.getPath(i) << endl;
		ofMesh mesh = loadObj(dir.getPath(i));
		if(dir.getName(i) == "Neutral.obj") {
			neutral = mesh;
		} else {
			blendshapes.push_back(mesh);
		}
	}
	
	for(int i = 0; i < blendshapes.size(); i++) {
		valid.push_back(vector<unsigned int>());
		for(int j = 0; j < blendshapes[i].getNumVertices(); j++) {
			blendshapes[i].getVertices()[j] -= neutral.getVertices()[j];
			if(blendshapes[i].getVertices()[j] != ofVec3f()) {
				valid[i].push_back(j);
			}
		}
	}
	
	light.enable();
	light.setPosition(+500, +500, +500);
}

void ofApp::update() {
	faceShift.update();
	
	current = neutral;
	// do a weighted sum of all offsets from neutral
	// (could precompute the offsets in the setup)
	float t = 3. * ofGetElapsedTimef();
	for(int i = 0; i < blendshapes.size(); i++) {
		float separation = PI;
		gui.set(i, ofClamp(ofSignedNoise(t + separation * i), 0, 1));
		
		float weight = gui.get(i);
		if(weight > 0) {
			for(int j = 0; j < valid[i].size(); j++) {
				int k = valid[i][j];
				current.getVertices()[k] += weight *  blendshapes[i].getVertices()[k];
			}
		}
	}
	buildNormals(current);
}

void ofApp::draw(){
	ofBackground(128);
	ofSetColor(255);
	ofNoFill();
	int n = faceShift.getBlendshapeCount(); 
	for(int i = 0; i < n; i++) {
		float weight = faceShift.getBlendshapeWeight(i);
		string name = faceShift.getBlendshapeName(i);
		ofRect(0, 0, weight * 100, 10);
		ofDrawBitmapString(name, weight * 100, 10);
		ofTranslate(0, 10);
	}
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofRotateX(180);
	current.draw();
	cam.end();
}
