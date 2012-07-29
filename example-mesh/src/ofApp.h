#pragma once

#include "ofMain.h"
#include "ofxMiniGui.h"
#include "ofxFaceShift.h"
#include "ofxAssimpModelLoader.h"

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
};

