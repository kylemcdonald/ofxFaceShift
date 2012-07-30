#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	faceShift.setup();
	faceShift.import("export");
	
	light.enable();
	light.setPosition(+500, +500, +500);
}

void ofApp::update() {
	faceShift.update();
}

void ofApp::draw(){
	ofBackground(128);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofRotateX(180);
	ofTranslate(faceShift.getPosition());
	ofScale(-1, 1, 1); // for some reason the rotation matrix x is flipped
	glMultMatrixf((GLfloat*) faceShift.getRotationMatrix().getPtr());
	ofScale(-1, 1, 1); // then we flip it back
	
	ofEnableLighting();
	ofSetColor(255);
	faceShift.getBlendMesh().draw();
	
	ofDisableLighting();
	ofSetColor(0);
	faceShift.getBlendMesh().drawWireframe();
	
	cam.end();
}
