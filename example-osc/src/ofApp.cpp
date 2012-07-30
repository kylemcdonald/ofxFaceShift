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
	
	addresses = ofSplitString(ofBufferFromFile("addresses.txt"), "\n");
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
	clearBundle();
	if(faceShift.update()) {
		lastPacket = ofGetElapsedTimef();
		timer.tick();
		if(faceShift.getFound()) {
			for(int i = 0; i < addresses.size(); i++) {
				addMessage("/gesture" + addresses[i], faceShift.getBlendshapeWeight(i));
			}
			addMessage("/gesture/eye/gaze/left", faceShift.getLeftEyeRotation());
			addMessage("/gesture/eye/gaze/right", faceShift.getRightEyeRotation());
			addMessage("/pose/position", faceShift.getPosition());
			addMessage("/pose/orientation", faceShift.getRotationEuler());
			addMessage("/timestamp", (float) faceShift.getTimestamp());
			addMessage("/found", 1);
		} else {
			addMessage("/found", 0);
		}
	}
	sendBundle();
}

void ofApp::draw(){
	float status = ofMap(ofGetElapsedTimef() - lastPacket, 0, 3, 0, 1, true);	
	ofColor magentaPrint = ofColor::fromHex(0xec008c);
	ofBackground((ofColor::gray).getLerped(magentaPrint, status));
	ofSetColor(255);
	ofDrawBitmapString("Listening on " + ofToString(faceShiftPort) + " at " + ofToString(timer.getFramerate(), 0) + " packets/second", 10, 20);
	ofDrawBitmapString("Sending to " + oscHost + ":" + ofToString(oscPort), 10, 40);
	if(status == 1) {
		ofDrawBitmapString("Not receiving any packets.", 10, 60);
	}
}
