#include "ofApp.h"

bool bSendPlz = true;

//--------------------------------------------------------------
void ofApp::setup(){
    // setup weather API
    weatherManager.setup();
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    
    // settings?
    rc::TextSettings serverSettings("serversettings.txt");
    options.port = ofToInt(serverSettings.getSetting("9000"));
    
    websocketServer.setup(options);
    websocketServer.addListener(this);
    
    bSendPlz = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( bSendPlz ){
        bSendPlz = false;
        
        manager.sendAllFiles( websocketServer, 40.708581, -73.952551);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event & m ){
    
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event & m ){
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event & m ){
    manager.removeConnection(m.conn);
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event & m ){
    if (m.isBinary){
        manager.addNewFile(m);
        bSendPlz = true;
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 's'){
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