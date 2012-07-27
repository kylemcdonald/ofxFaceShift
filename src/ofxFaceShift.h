#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxFaceShift {
public:
	void setup(unsigned int port = 33433);
	bool update();
	
	unsigned int getExpressionCount() const;
	float getExpressionWeight(unsigned int i) const;
	string getExpressionName(unsigned int i) const;

	const vector<float>& getExpressionWeights() const;
	const vector<string>& getExpressionNames() const;
	
protected:
	ofxUDPManager udpConnection;
	
	unsigned short versionNumber;
	double timestamp;
	bool success;
	
	ofQuaternion rotation;
	ofVec3f translation;
	ofVec2f leftEye, rightEye;
	
	vector<ofVec3f> markers;
	vector<float> expressionWeights;
	vector<string> expressionNames;
};

