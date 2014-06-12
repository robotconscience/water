#pragma once

#include "ofMain.h"
#include "ofxCoreLocation.h"

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
    
        // remove me!
        WeatherManager weatherAPI;
        void onEvaporationLookup( int & value );
};
