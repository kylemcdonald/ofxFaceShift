#pragma once

#include "ofMain.h"
#include "ofxFaceShift.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	ofxFaceShift faceShift;
	
	ofEasyCam cam;
	ofLight light;
	
	ofMesh neutral, current;
	vector<ofMesh> blendshapes;
	ofVec3f leftEye, rightEye;
	vector<vector<unsigned int> > valid;
};

