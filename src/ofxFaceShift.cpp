#include "ofxFaceShift.h"

enum {
	FS_FRAME_INFO_BLOCK = 101,
	FS_POSE_BLOCK = 102,
	FS_BLENDSHAPES_BLOCK = 103,
	FS_EYES_BLOCK = 104,
	FS_MARKERS_BLOCK = 105
};

template <class T>
void readRaw(stringstream& stream, T& data, int n) {
	stream.read((char*) &data, n);
}

template <class T>
void readRaw(stringstream& stream, T& data) {
	readRaw(stream, data, sizeof(T));
}

void ofxFaceShift::setup(unsigned int port) {	
	udpConnection.Create();
	udpConnection.Bind(port);
	udpConnection.SetNonBlocking(true);
	
	expressionNames = ofSplitString(ofBufferFromFile("blendshapes.txt"), "\n");
}

void ofxFaceShift::update() {
	const unsigned int maxSize = 1024;
	char message[maxSize];
	int n = udpConnection.Receive(message, maxSize);
	if(n > 0) {
		stringstream data;
		data.write(message, n);
		
		unsigned short blockID, versionNumber;
		unsigned int blockSize;
		
		// data container
		unsigned short numberBlocks;
		readRaw(data, blockID);
		readRaw(data, versionNumber);
		readRaw(data, blockSize);
		readRaw(data, numberBlocks);
		cout << "data container " << blockID << "/" << versionNumber << "/" << blockSize << "/" << numberBlocks << endl;
		
		double timestamp;
		bool success;
		float prx, pry, prz, prw;
		float ptx, pty, ptz;
		expressionWeights.clear();
		float leftEyeTheta, leftEyePhi, rightEyeTheta, rightEyePhi;
		vector<ofVec3f> markers;
		
		for(int i = 0; i < numberBlocks; i++) {
			readRaw(data, blockID);
			readRaw(data, versionNumber);
			readRaw(data, blockSize);
			cout << "block " << i << ": " << blockID << "/" << versionNumber << "/" << blockSize << endl;
			
			switch(blockID) {
				case FS_FRAME_INFO_BLOCK:
					readRaw(data, timestamp);
					readRaw(data, success);
					break;
				case FS_POSE_BLOCK:
					// not sure if it's xyzw or wxyz
					readRaw(data, prx);
					readRaw(data, pry);
					readRaw(data, prz);
					readRaw(data, prw);
					readRaw(data, ptx);
					readRaw(data, pty);
					readRaw(data, ptz);
					break;
				case FS_BLENDSHAPES_BLOCK:
					unsigned int ncoeffs;
					readRaw(data, ncoeffs);
					for(int i = 0; i < ncoeffs; i++) {
						float cur;
						readRaw(data, cur);
						expressionWeights.push_back(cur);
					}
					break;
				case FS_EYES_BLOCK:
					readRaw(data, leftEyeTheta);
					readRaw(data, leftEyePhi);
					readRaw(data, rightEyeTheta);
					readRaw(data, rightEyePhi);
					break;
				case FS_MARKERS_BLOCK:
					unsigned short nmarkers;
					readRaw(data, nmarkers);
					for(int i = 0; i < nmarkers; i++) {
						ofVec3f cur;
						readRaw(data, cur.x);
						readRaw(data, cur.y);
						readRaw(data, cur.z);
						markers.push_back(cur);
					}
					break;
			}
		}
	}
}

void ofxFaceShift::draw(){
	ofBackground(0);
	ofColor(255);
	ofNoFill();
	for(int i = 0; i < expressionWeights.size(); i++) {
		ofRect(0, 0, expressionWeights[i] * 100, 10);
		ofDrawBitmapString(expressionNames[i], expressionWeights[i] * 100, 10);
		ofTranslate(0, 10);
	}
}

unsigned int ofxFaceShift::getExpressionCount() const {
	return expressionWeights.size();
}

float ofxFaceShift::getExpressionWeight(unsigned int i) const {
	return expressionWeights.at(i);
}

string ofxFaceShift::getExpressionName(unsigned int i) const {
	return expressionNames.at(i);
}

const vector<float>& ofxFaceShift::getExpressionWeights() const {
	return expressionWeights;
}

const vector<string>& ofxFaceShift::getExpressionNames() const {
	return expressionNames;
}
