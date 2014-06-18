#include "ofApp.h"

bool bSendPlz = true;

//--------------------------------------------------------------
void ofApp::setup(){
    // setup weather API
    weatherManager.setup();
    ofAddListener(weatherManager.onEvaporationLookup, this, &ofApp::onEvaporationLookup);
    
    // setup websocket server
    ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
    
    rc::TextSettings serverSettings("serversettings.txt");
    options.port = ofToInt(serverSettings.getSetting("9000")); // settings?
    
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
void ofApp::onEvaporationLookup( EvapPrecipResponse & value ){
    // find connection
    if ( processingConnections.count(value.response_id) != 0 ){
        cout << value.response_id << endl;
        ofxLibwebsockets::Connection * c = processingConnections[value.response_id];
        processingConnections.erase(value.response_id);
        
        stringstream json;
        json<<"{\"precipitation\":\""<<value.precipitation<< "\",";
        json<<"\"evapotranspiration\":\""<<value.evapotranspiration<< "\"}";
        
        c->send(json.str());
        
    } else {
        ofLogError()<<"Got some bullshit request id "<<value.response_id;
    }
    
    
}

#pragma mark ofxLibwebsocket Events

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
    } else {
        if ( !m.json.isNull() ){
            float lat = ofToFloat(m.json["lat"].asString());
            float lon = ofToFloat(m.json["long"].asString());
            int req_id = weatherManager.lookupEvaporation(lat, lon);
            processingConnections[req_id] = &m.conn;
        }
    }
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event & m ){}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){}

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