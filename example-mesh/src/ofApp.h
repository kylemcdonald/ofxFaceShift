#pragma once

#include "ofMain.h"
#include "ofxMiniGui.h"
#include "ofxFaceShift.h"

using namespace ofxMiniGui;

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	ofxFaceShift faceShift;
	
	ofEasyCam cam;
	ofLight light;
	
	Gui gui;
	
	ofMesh neutral, current;
	vector<ofMesh> blendshapes;
	ofVec3f leftEye, rightEye;
	vector<vector<unsigned int> > valid;
};

