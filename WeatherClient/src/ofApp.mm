#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofAddListener(locationGetter.onLocationRetrieved, this, &ofApp::onLocation);
    ofAddListener(locationGetter.onError, this, &ofApp::onError);
    
    locationGetter.getLocation();
    
    websocket.connect("localhost", 9000);
    websocket.addListener(this);
    
    precipitation = evapotranspiration = -1;
}

//--------------------------------------------------------------
void ofApp::update(){
    if ( precipitation != -1 && evapotranspiration != -1 ){
        if ( evapoTimer - evapoLast > evaporateFileEvery ){
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
            evapoTimer = evapoLast = ofGetElapsedTimeMillis();
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::onLocation( Location & loc ){
    currentLocation = loc;
    cout <<"got location"<<endl;
    
    // HERE: SEND!
    stringstream ss;
    ss<<"{\"lat\":\""<<loc.latitude<<"\",\"long\":\""<< loc.longitude<< "\"}";
    websocket.send(ss.str());
}

//--------------------------------------------------------------
void ofApp::onError( string & event ){
    cout << "error "<<event << endl;
    locationGetter.getLocation();
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
    } else {
        if ( !m.json.isNull() ){
            precipitation = ofToFloat( m.json["precipitation"].asString());
            evapotranspiration = ofToFloat( m.json["evapotranspiration"].asString());
            
            // 1 inch = 1 file/second
            evaporateFileEvery = ofMap(evapotranspiration, 0.0, 1.0, /*24 * 60 **/ 60 * 1000, 1000);
            cout << evaporateFileEvery << endl;
            evapoTimer = evapoLast = ofGetElapsedTimeMillis();
        } else {
            cout <<"fucked json"<<endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
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
