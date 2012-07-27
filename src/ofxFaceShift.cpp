#include "ofxFaceShift.h"

const unsigned int maxPacketSize = 1024;

enum {
	FS_FRAME_INFO_BLOCK = 101,
	FS_POSE_BLOCK = 102,
	FS_BLENDSHAPES_BLOCK = 103,
	FS_EYES_BLOCK = 104,
	FS_MARKERS_BLOCK = 105
};

template <class T>
void readRaw(stringstream& stream, T& data) {
	stream.read((char*) &data, sizeof(T));
}

void ofxFaceShift::setup(unsigned int port) {	
	udpConnection.Create();
	udpConnection.Bind(port);
	udpConnection.SetNonBlocking(true);
	
	expressionNames = ofSplitString(ofBufferFromFile("blendshapes.txt"), "\n");
}

bool ofxFaceShift::update() {
	bool newFrame = false;
	static char message[maxPacketSize];
	int messageLength = udpConnection.Receive(message, maxPacketSize);
	if(messageLength > 0) {
		newFrame = true;
		markers.clear();
		expressionWeights.clear();
	
		stringstream data;
		data.write(message, messageLength);
		
		unsigned short blockID;
		unsigned int blockSize;
		
		readRaw(data, blockID);
		readRaw(data, versionNumber);
		readRaw(data, blockSize);
		
		unsigned short numberBlocks;
		readRaw(data, numberBlocks);
		
		for(int i = 0; i < numberBlocks; i++) {
			readRaw(data, blockID);
			readRaw(data, versionNumber);
			readRaw(data, blockSize);
			
			switch(blockID) {
				case FS_FRAME_INFO_BLOCK:
					readRaw(data, timestamp);
					readRaw(data, success);
					break;
				case FS_POSE_BLOCK:
					readRaw(data, rotation.x());
					readRaw(data, rotation.y());
					readRaw(data, rotation.z());
					readRaw(data, rotation.w());
					readRaw(data, translation.x);
					readRaw(data, translation.y);
					readRaw(data, translation.z);
					break;
				case FS_BLENDSHAPES_BLOCK:
					unsigned int expressionCount;
					readRaw(data, expressionCount);
					for(int i = 0; i < expressionCount; i++) {
						float expressionWeight;
						readRaw(data, expressionWeight);
						expressionWeights.push_back(expressionWeight);
					}
					break;
				case FS_EYES_BLOCK:
					readRaw(data, leftEye.x);
					readRaw(data, leftEye.y);
					readRaw(data, rightEye.x);
					readRaw(data, rightEye.y);
					break;
				case FS_MARKERS_BLOCK:
					unsigned short markerCount;
					readRaw(data, markerCount);
					for(int i = 0; i < markerCount; i++) {
						ofVec3f marker;
						readRaw(data, marker.x);
						readRaw(data, marker.y);
						readRaw(data, marker.z);
						markers.push_back(marker);
					}
					break;
			}
		}
	}
	return newFrame;
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
