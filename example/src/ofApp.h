#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "ofxFaceShift.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	ofxFaceShift faceShift;
};

