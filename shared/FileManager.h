//
//  FileManager.h
//  WeatherServer
//
//  Created by Brett Renfer on 6/12/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxLibwebsockets.h" // this should be JSON?
#include "ofxXmlSettings.h" // this should be JSON?

#include "FileUtils.h"

class FileManager {
public:
    
    FileManager();
    
    void update( ofEventArgs & e );
    
    // debug method
    void sendAllFiles( ofxLibwebsockets::Server & server, float lat, float lon );
    
    // accepts event from ofxLws for now
    void addNewFile( ofxLibwebsockets::Event & e );
    vector<string> & getFiles( float lat, float lon );
    
    // lookup table = -90 to 90     (lat) mapped from 0 to 1800
    int getLookupLat( float lat );
    
    // lookup table = -180 to 180   (long) mapped from 0 to 3600
    int getLookupLong( float lon );
    
    // manage connections
    void addConnection( ofxLibwebsockets::Connection & c, float lat, float lon );
    vector<ofxLibwebsockets::Connection* > & getConnections( float lat, float lon );
    void removeConnection( ofxLibwebsockets::Connection & c );
    
protected:
    map< int, map<int, vector<string> > > latLongMap;
    map< int, map<int, vector<ofxLibwebsockets::Connection* > > > latLongClientMap;

    ofBuffer fileBuffer, outputBuffer; // output buffers
};