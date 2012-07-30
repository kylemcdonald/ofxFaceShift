#include "ofApp.h"

const float eyeRadius = 14, corneaScale = .5;
void drawEye(ofVec3f position, ofVec2f orientation = ofVec2f()) {
	ofPushMatrix();
	ofTranslate(position);
	// i'm not sure if this rotation is mapped correctly
	ofRotateY(orientation.y);
	ofRotateX(orientation.x);
	ofSphere(eyeRadius);
	ofTranslate(0, 0, -eyeRadius * (1.1 - corneaScale));
	ofSphere(eyeRadius * corneaScale);
	ofPopMatrix();
}

void ofApp::setup() {
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
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
	ofSetColor(255);
	
	cam.begin();
	glEnable(GL_DEPTH_TEST);
	ofFill();
	ofRotateX(180);
	ofTranslate(faceShift.getPosition());
	ofScale(-1, 1, 1); // for some reason the rotation matrix x is flipped
	glMultMatrixf((GLfloat*) faceShift.getRotationMatrix().getPtr());
	ofScale(-1, 1, 1); // then we flip it back
	
	ofEnableLighting();
	faceShift.getBlendMesh().draw();
	drawEye(faceShift.getLeftEyeOffset(), faceShift.getLeftEyeRotation());
	drawEye(faceShift.getRightEyeOffset(), faceShift.getRightEyeRotation());
	
	ofDisableLighting();
	ofSetColor(0);
	faceShift.getBlendMesh().drawWireframe();
	
	cam.end();
}
