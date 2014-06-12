#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofAddListener(locationGetter.onLocationRetrieved, this, &ofApp::onLocation);
    ofAddListener(locationGetter.onError, this, &ofApp::onError);
    
    locationGetter.getLocation();
    weatherAPI.setup();
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
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
