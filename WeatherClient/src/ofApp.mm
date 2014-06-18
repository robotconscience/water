#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofAddListener(locationGetter.onLocationRetrieved, this, &ofApp::onLocation);
    ofAddListener(locationGetter.onError, this, &ofApp::onError);
    
    locationGetter.getLocation();
    weatherAPI.setup();
    
    websocket.connect("localhost", 9000);
    websocket.addListener(this);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::onLocation( Location & loc ){
    currentLocation = loc;
    cout <<"got location"<<endl;
    weatherAPI.lookupEvaporation(loc.latitude, loc.longitude);
}

//--------------------------------------------------------------
void ofApp::onError( string & event ){
    cout << "error "<<event << endl;
}

//--------------------------------------------------------------
void ofApp::onEvaporationLookup( int & value ){
    cout << "evaporation: "<<value<<endl;
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event & m ){
    
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event & m ){
    
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event & m ){
    
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event & m ){
    
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event & m ){
    if (m.isBinary){
        fileHandler.receive(m);
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 's' ){
        cout <<"SEND"<<endl;
        ofDirectory dir;
        dir.allowExt("jpg");
        int num = dir.listDir("files");
        if ( num > 0 ){
            int ind = floor( ofRandom(0, num));
            fileHandler.send(dir.getPath(ind), "testClient", currentLocation.latitude, currentLocation.longitude, websocket);
        } else {
            cout <<"NO FILES"<<endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
