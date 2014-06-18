#pragma once

#include "ofMain.h"

// addons
#include "ofxFluid.h"
#include "ofxLibwebsockets.h"
//#include "ofxSpacebrew.h"
#include "ofxRCUtils.h"
#include "ofxLabFlexParticleSystem.h"

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
    
        // wind simulation
        ofLight light;
        ofMesh sphere;
        ofxFluid fluid;
        
        ofVec2f oldM;
        int     width,height;
        bool    bPaint, bObstacle, bBounding, bClear;
    
        ofPixels velPix;
        bool    bNeedToRead;
        ofVec2f getVelocityLatLong( float lat, float lon );
        ofVec2f getVelocity( float x, float y );
    
        // weather API manager
        WeatherManager weatherManager;
    
        // files
        ofFbo fileFBO, screenFBO;
        lab::ParticleSystem files;
};
