#include "ofMain.h"
#include "ofApp.h"
#include "ofAppNoWindow.h"

//========================================================================
int main( ){
    ofAppNoWindow window;
    ofSetupOpenGL(&window, 1, 1, OF_WINDOW);
	ofRunApp(new ofApp());

}
