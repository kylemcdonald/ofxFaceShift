#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	faceShift.setup();
	faceShift.import("export");
	
	light.enable();
	light.setPosition(+500, +500, +500);
}

void ofApp::update() {
	faceShift.randomUpdate();
}

void ofApp::draw(){
	ofBackground(255);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofRotateX(180);
	
	//ofEnableLighting();
	ofSetColor(255);
	ofMesh& mesh = faceShift.getBlendMesh();
	mesh.draw();
	
	ofDisableLighting();
	ofSetColor(0);
	mesh.drawWireframe();
	
	ofSeedRandom(0);
	ofMesh spikes;
	spikes.setMode(OF_PRIMITIVE_LINES);
	for(int i = 0; i < mesh.getNumIndices();) {
		unsigned int i1 = mesh.getIndex(i++), i2 = mesh.getIndex(i++), i3 = mesh.getIndex(i++);
		ofVec3f& normal = mesh.getNormals()[i1];
		ofVec3f& v1 = mesh.getVertices()[i1];
		ofVec3f& v2 = mesh.getVertices()[i2];
		ofVec3f& v3 = mesh.getVertices()[i3];
		float len = (v1.distance(v2) + v2.distance(v3) + v3.distance(v1)) / 3;
		ofVec3f target = (v1 + v2 + v3) / 3. + normal * len;// * ofRandom(8);
		spikes.addVertex(target);
		spikes.addVertex(v1);
		spikes.addVertex(target);
		spikes.addVertex(v2);
		spikes.addVertex(target);
		spikes.addVertex(v3);
	}
	
	spikes.draw();
	
	cam.end();
}
