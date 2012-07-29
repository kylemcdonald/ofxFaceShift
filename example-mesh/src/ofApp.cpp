#include "ofApp.h"

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
		ofxAssimpModelLoader loader;
		loader.loadModel(dir.getPath(i));
		ofMesh mesh = loader.getMesh(0);
		if(dir.getName(i) == "Neutral.obj") {
			neutral = mesh;
		} else {
			blendshapes.push_back(mesh);
		}
	}
	
	light.enable();
	light.setPosition(+500, 0, 0);
}

void ofApp::update() {
	faceShift.update();
	
	current = neutral;
	// zero all vertices
	for(int i = 0; i < current.getNumVertices(); i++) {
		//current.getVertices()[i].set(0);
	}
	// do a weighted sum of all offsets from neutral
	// (could precompute the offsets in the setup)
	float weightSum = 1;
	for(int i = 0; i < blendshapes.size(); i++) {
		float weight = gui.get(i);
		weightSum += weight;
		if(weight > 0) {
			for(int j = 0; j < current.getNumVertices(); j++) {
				//ofVec3f offset = blendshapes[i].getVertices()[j] - neutral.getVertices()[j];
				//current.getVertices()[j] += weight * offset;
				current.getVertices()[j] += weight * blendshapes[i].getVertices()[j];
			}
		}
	}
	// go back through and add the neutral vertices and normalize the weights
	for(int i = 0; i < current.getNumVertices(); i++) {
		current.getVertices()[i] /= weightSum;
		//current.getVertices()[i] += neutral.getVertices()[i];
	}
}

void ofApp::draw(){
	ofBackground(0);
	ofColor(255);
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
	ofMesh* curMesh;
	if(ofGetMousePressed()) {
		curMesh = &current;
	} else {
		curMesh = &blendshapes[(int) ofMap(mouseX, 0, ofGetWidth(), 0, blendshapes.size() - 1, true)];
	}
	curMesh->draw();
	for(int i = 0; i < curMesh->getNumVertices(); i += 100) {
		ofDrawBitmapString(ofToString(i), curMesh->getVertices()[i]);
	}
	cam.end();
}
