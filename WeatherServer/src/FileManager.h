//
//  FileManager.h
//  WeatherServer
//
//  Created by Brett Renfer on 6/12/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h" // this should be JSON?

#include "FileUtils.h"

// for now just accepting jpegs?
#include "ofxTurboJpeg.h"

class FileManager {
public:
    
    FileManager(){
        ofAddListener( ofEvents().update, this, &FileManager::update );
    }
    
    void update( ofEventArgs & e ){
        
    }
    
    // debug method
    void sendAllFiles( ofxLibwebsockets::Server & server, float lat, float lon ){
        vector<string> & files = getFiles(lat, lon);
        
        for ( auto & path : files ){
            ofFile file(path, ofFile::ReadWrite);
            fileBuffer = file.readToBuffer();
            file.remove();
            
            // can call as void since just writing to outputBuffer
            getSendBuffer( fileBuffer, outputBuffer, "server", lat, lon);
            server.sendBinary( outputBuffer ); // to-do: use client map here!
        }
        files.clear();
    }
    
    // accepts event from ofxLws for now
    void addNewFile( ofxLibwebsockets::Event & e ){
        
        // to-do: use e.connection to look up client!
        
        float lat, lon;
        string name;
        ofBuffer binary;
        
        parseWeatherEvent( e, binary, lat, lon, name );
        
        int lookupLat = getLookupLat(lat);
        int lookupLon = getLookupLong(lon);
        
        // set up file map
        if ( latLongMap.count(lookupLat) == 0){
            latLongMap[lookupLat] = map<int, vector<string> >();
        }
        if( latLongMap[lookupLat].count(lookupLon == 0) ){
            latLongMap[lookupLat][lookupLon] = vector<string>();
        }
        
        ofFile file;
        file.open( "files/" + ofGetTimestampString() + ".jpg", ofFile::WriteOnly );
        bool bWritten = file.writeFromBuffer(binary);
        cout <<lat <<":"<<lon <<endl;
        
        latLongMap[lookupLat][lookupLon].push_back(file.path());
    }
    
    // lookup table = -90 to 90     (lat) mapped from 0 to 1800
    int getLookupLat( float lat ){
        return ofMap(lat, -90, 90, 0, 3600, true);
    }
    
    // lookup table = -180 to 180   (long) mapped from 0 to 3600
    int getLookupLong( float lon ){
        return ofMap(lon, -180, 180, 0, 1800, true);
    }
    
    vector<string> & getFiles( float lat, float lon ){
        int lookupLat = getLookupLat(lat);
        int lookupLon = getLookupLong(lon);
        
        static vector<string> emptyList;
        
        if ( latLongMap.count(lookupLat) == 0 || latLongMap[lookupLat].count(lookupLon == 0) ){
            return emptyList;
        } else {
            return latLongMap[lookupLat][lookupLon];
        }
    }
    
protected:
    ofxTurboJpeg turboJpeg;
    map< int, map<int, vector<string> > > latLongMap;
    map< int, map<int, vector<ofxLibwebsockets::Client* > > > latLongClientMap;

    ofBuffer fileBuffer, outputBuffer; // output buffers
};