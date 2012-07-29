#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxFaceShift {
public:
	void setup(unsigned int port = 33433);
	bool update();
	
	unsigned int getBlendshapeCount() const;
	float getBlendshapeWeight(unsigned int i) const;
	const vector<float>& getBlendshapeWeights() const;
	string getBlendshapeName(unsigned int i) const;
	const vector<string>& getBlendshapeNames() const;
	
	unsigned int getMarkerCount() const;
	ofVec3f getMarker(unsigned int i) const;
	const vector<ofVec3f>& getMarkers() const;
	
	unsigned short getVersionNumber() const; // fsstudio protocol version
	double getTimestamp() const; // in milliseconds
	bool getFound() const;
	
	ofQuaternion getRotation() const;
	ofVec3f getRotationEuler() const;
	ofMatrix4x4 getRotationMatrix() const;
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
	vector<float> blendshapeWeights;
	vector<string> blendshapeNames;
};

