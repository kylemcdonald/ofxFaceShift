#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxFaceShift {
public:
	ofxFaceShift();

	void setup(unsigned int port = 33433);
	void import(string modelFolder);
	bool update();
	
	unsigned int getBlendshapeCount() const;
	float getBlendshapeWeight(unsigned int i) const;
	float getBlendshapeWeight(const string& blendshapeName) const;
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
	ofVec2f getLeftEyeRotation() const;
	ofVec2f getRightEyeRotation() const;
	ofVec3f getLeftEyeOffset() const;
	ofVec3f getRightEyeOffset() const;
	
	ofMesh& getBlendMesh();
	ofMesh& getBlendshapeMesh(unsigned int i);
	
protected:
	ofxUDPManager udpConnection;
	
	unsigned short versionNumber;
	double timestamp;
	bool found;
	
	ofQuaternion rotation;
	ofVec3f position;
	ofVec2f leftEyeRotation, rightEyeRotation;
	
	vector<ofVec3f> markers;
	vector<float> blendshapeWeights;
	vector<string> blendshapeNames;
	
	bool imported, blendNeedsUpdating;
	ofMesh neutralMesh, blendMesh;
	vector<ofMesh> blendshapeMeshes;
	ofVec3f leftEyeOffset, rightEyeOffset;
	vector<vector<unsigned int> > validBlendPoints;
};

