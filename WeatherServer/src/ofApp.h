#pragma once

#include "ofMain.h"

// addons
#include "ofxLibwebsockets.h"
//#include "ofxSpacebrew.h"
#include "ofxRCUtils.h"

// project code
#include "WeatherManager.h"
#include "FileManager.h"

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

        // server
        FileManager manager;
        ofxLibwebsockets::Server websocketServer;
    
        // weather API manager
        WeatherManager weatherManager;
    
        void onConnect( ofxLibwebsockets::Event & m );
        void onOpen( ofxLibwebsockets::Event & m );
        void onClose( ofxLibwebsockets::Event & m );
        void onIdle( ofxLibwebsockets::Event & m );
        void onMessage( ofxLibwebsockets::Event & m );
        void onBroadcast( ofxLibwebsockets::Event & m );
};
