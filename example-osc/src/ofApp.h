#pragma once

#include "ofMain.h"
#include "ofxFaceShift.h"
#include "ofxOsc.h"
#include "RateTimer.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
		
	void loadSettings();
	
	void clearBundle();
	template <class T>
	void addMessage(string address, T data);
	void sendBundle();
	
	RateTimer timer;
	float lastPacket;
	unsigned int faceShiftPort;
	ofxFaceShift faceShift;
	ofEasyCam cam;
	
	string oscHost;
	int oscPort;
	ofxOscSender osc;
	ofxOscBundle bundle;
	vector<string> addresses;
};

