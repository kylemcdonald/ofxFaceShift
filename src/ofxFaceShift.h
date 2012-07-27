#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxFaceShift {
public:
	void setup(unsigned int port = 33433);
	void update();
	void draw();
	
	unsigned int getExpressionCount() const;
	float getExpressionWeight(unsigned int i) const;
	string getExpressionName(unsigned int i) const;

	const vector<float>& getExpressionWeights() const;
	const vector<string>& getExpressionNames() const;
	
protected:
	ofxUDPManager udpConnection;
	vector<float> expressionWeights;
	vector<string> expressionNames;
};

