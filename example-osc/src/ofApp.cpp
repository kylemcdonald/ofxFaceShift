#include "ofApp.h"

#include "ofxXmlSettings.h"
void ofApp::loadSettings() {
	ofxXmlSettings xml;
	xml.loadFile("settings.xml");

	xml.pushTag("osc");
	oscHost = xml.getValue("host", "localhost");
	oscPort = xml.getValue("port", 8338);
	xml.popTag();
	
	xml.pushTag("faceShift");
	faceShiftPort = xml.getValue("port", 33433);
	xml.popTag();
	
	ofFile file("addresses.txt");
	while(!file.eof()) {
		string cur;
		getline(file, cur);
		addresses.push_back(cur);
	}
	cout << ofToString(addresses) << "/" << addresses.size() << endl;
}

void ofApp::clearBundle() {
	bundle.clear();
}

template <>
void ofApp::addMessage(string address, ofVec3f data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data.x);
	msg.addFloatArg(data.y);
	msg.addFloatArg(data.z);
	bundle.addMessage(msg);
}

template <>
void ofApp::addMessage(string address, ofVec2f data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data.x);
	msg.addFloatArg(data.y);
	bundle.addMessage(msg);
}

template <>
void ofApp::addMessage(string address, float data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addFloatArg(data);
	bundle.addMessage(msg);
}

template <>
void ofApp::addMessage(string address, int data) {
	ofxOscMessage msg;
	msg.setAddress(address);
	msg.addIntArg(data);
	bundle.addMessage(msg);
}

void ofApp::sendBundle() {
	osc.sendBundle(bundle);
}

void ofApp::setup() {
	ofSetVerticalSync(true);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	
	loadSettings();
	
	faceShift.setup(faceShiftPort);
	osc.setup(oscHost, oscPort);
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
	
	ofVec2f leftEye = faceShift.getLeftEyeRotation();
	ofVec2f rightEye = faceShift.getRightEyeRotation();
	ofVec3f pos = faceShift.getPosition();
	ofVec3f rot = faceShift.getRotationEuler();
}
