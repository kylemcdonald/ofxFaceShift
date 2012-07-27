/*
 todo:
 - euler angles from rotation quaternion
 - rotation matrix from rotation quaternion
 - n-vec from left/right eye orientation
*/

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxFaceShift {
public:
	void setup(unsigned int port = 33433);
	bool update();
	
	unsigned int getExpressionCount() const;
	float getExpressionWeight(unsigned int i) const;
	const vector<float>& getExpressionWeights() const;
	string getExpressionName(unsigned int i) const;
	const vector<string>& getExpressionNames() const;
	
	unsigned int getMarkerCount() const;
	ofVec3f getMarker(unsigned int i) const;
	const vector<ofVec3f>& getMarkers() const;
	
	unsigned short getVersionNumber() const; // fsstudio protocol version
	double getTimestamp() const; // in milliseconds
	bool getFound() const;
	
	ofQuaternion getRotation() const;
	ofVec3f getPosition() const; // millimeters by default, specify in fsstudio
	ofVec2f getLeftEye() const;
	ofVec2f getRightEye() const;
	
protected:
	ofxUDPManager udpConnection;
	
	unsigned short versionNumber;
	double timestamp;
	bool found;
	
	ofQuaternion rotation;
	ofVec3f position;
	ofVec2f leftEye, rightEye;
	
	vector<ofVec3f> markers;
	vector<float> expressionWeights;
	vector<string> expressionNames;
};

