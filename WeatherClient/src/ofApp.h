#pragma once

#include "ofMain.h"
#include "ofxCoreLocation.h"
#include "ofxLibwebsockets.h"
#include "FileClient.h"

// to be removed
#include "WeatherManager.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        // GPS stuff
        ofxCoreLocation locationGetter;
        Location        currentLocation;
        void onLocation( Location & loc );
        void onError( string & event );
    
        // file handling
        FileClient fileHandler;
    
        float precipitation, evapotranspiration;
        int evaporateFileEvery, evapoTimer, evapoLast;
    
        // websocket stuff
        ofxLibwebsockets::Client websocket;
        void onConnect( ofxLibwebsockets::Event & m );
        void onOpen( ofxLibwebsockets::Event & m );
        void onClose( ofxLibwebsockets::Event & m );
        void onIdle( ofxLibwebsockets::Event & m );
        void onMessage( ofxLibwebsockets::Event & m );
        void onBroadcast( ofxLibwebsockets::Event & m );
};
