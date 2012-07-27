#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main( ){
  ofAppGlutWindow window;
	ofSetupOpenGL(&window, 400, 46 * 10, OF_WINDOW);
	ofRunApp(new testApp());
}
