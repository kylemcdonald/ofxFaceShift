#pragma once

#include "ofMain.h"
#include "ofxFaceShift.h"

class RandomFaceShift : public ofxFaceShift {
public:
	void randomUpdate() {
		float t = ofGetElapsedTimef();
		blendshapeWeights.clear();
		unsigned int blendshapeCount = blendshapeNames.size();
		ofSeedRandom(0);
		for(int i = 0; i < blendshapeCount; i++) {
			float blendshapeWeight = pow(ofNoise(t * ofRandom(1, 4) + i * 10. * PI), 3);
			blendshapeWeights.push_back(blendshapeWeight);
		}
		blendNeedsUpdating = true;
	}
};

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	RandomFaceShift faceShift;
	
	ofEasyCam cam;
	ofLight light;
};

