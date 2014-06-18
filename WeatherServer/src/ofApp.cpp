#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableAlphaBlending();
    ofSetCircleResolution(100);
    
    // setup weather API
    weatherManager.setup();
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    
    // settings?
    rc::TextSettings serverSettings("serversettings.txt");
    options.port = ofToInt(serverSettings.getSetting("9000"));
    
    websocketServer.setup(options);
    websocketServer.addListener(this);
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor::gray, ofColor::black, OF_GRADIENT_LINEAR);
    ofDisableDepthTest();
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
        manager.addNewFile(m);
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 's'){
        manager.sendAllFiles( websocketServer, 40.708581, -73.952551);
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