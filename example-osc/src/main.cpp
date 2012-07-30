#include "ofApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 400, 100, OF_WINDOW);
	ofRunApp(new ofApp());
}
