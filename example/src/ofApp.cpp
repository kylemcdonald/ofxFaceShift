#include "ofApp.h"

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	faceShift.setup();
}

void ofApp::update() {
	faceShift.update();
}

void ofApp::draw(){
	ofBackground(0);
	ofColor(255);
	ofFill();
	int n = faceShift.getBlendshapeCount(); 
	ofPushMatrix();
	for(int i = 0; i < n; i++) {
		float weight = faceShift.getBlendshapeWeight(i);
		string name = faceShift.getBlendshapeName(i);
		ofRect(0, 0, weight * 100, 10);
		ofDrawBitmapString(name, weight * 100, 10);
		ofTranslate(0, 10);
	}
	ofPopMatrix();
	
	float eyeArea = 150, eyeZoom = -4;
	ofNoFill();
	ofTranslate(0, ofGetHeight() - eyeArea);
	ofRect(0, 0, eyeArea, eyeArea);
	ofPushMatrix();
	ofVec2f leftEye = faceShift.getLeftEyeRotation();
	ofTranslate(eyeArea / 2, eyeArea / 2);
	ofTranslate(eyeZoom * leftEye);
	ofCircle(0, 0, 6);
	ofDrawBitmapString(ofToString(leftEye.x, 2) + "\n" + ofToString(leftEye.y, 2), 12, 0);
	ofPopMatrix();
	
	ofTranslate(eyeArea, 0);
	ofRect(0, 0, eyeArea, eyeArea);
	ofPushMatrix();
	ofVec2f rightEye = faceShift.getRightEyeRotation();
	ofTranslate(eyeArea / 2, eyeArea / 2);
	ofTranslate(eyeZoom * rightEye);
	ofCircle(0, 0, 6);
	ofDrawBitmapString(ofToString(rightEye.x, 2) + "\n" + ofToString(rightEye.y, 2), 12, 0);
	ofPopMatrix();
	
	cam.begin();
	ofRotateX(180);
	ofVec3f pos = faceShift.getPosition();
	ofVec3f rot = faceShift.getRotationEuler();
	string posStr = ofToString(pos.x, 0) + "/" + ofToString(pos.y, 0) + "/" + ofToString(pos.z, 0);
	string rotStr = ofToString(rot.x, 0) + "/" + ofToString(rot.y, 0) + "/" + ofToString(rot.z, 0);
	ofTranslate(pos);
	ofDrawBitmapString("pos: " + posStr + "\n" + "rot: " + rotStr, 50, 50);
	ofScale(-1, 1, 1); // for some reason the rotation matrix x is flipped
	glMultMatrixf((GLfloat*) faceShift.getRotationMatrix().getPtr());
	ofScale(-1, 1, 1); // then we flip it back
	ofNoFill();
	ofBox(64);
	ofDrawAxis(128);
	cam.end();
}
