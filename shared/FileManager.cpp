//
//  FileManager.cpp
//  WeatherServer
//
//  Created by Brett Renfer on 6/18/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "FileManager.h"

//--------------------------------------------------------------
FileManager::FileManager(){
    ofAddListener( ofEvents().update, this, &FileManager::update );
}

//--------------------------------------------------------------
void FileManager::update( ofEventArgs & e ){
    
}

//--------------------------------------------------------------
void FileManager::sendAllFiles( ofxLibwebsockets::Server & server, float lat, float lon ){
    vector<string> & files = getFiles(lat, lon);
    
    for ( auto & path : files ){
        ofFile file(path, ofFile::ReadWrite);
        fileBuffer = file.readToBuffer();
        file.remove();
        
        // can call as void since just writing to outputBuffer
        getSendBuffer( fileBuffer, outputBuffer, "server", lat, lon);
        vector<ofxLibwebsockets::Connection*> & conn = getConnections(lat, lon);
        for ( auto & c : conn ){
            c->sendBinary( outputBuffer );
        }
    }
    files.clear();
}

//--------------------------------------------------------------
void FileManager::addNewFile( ofxLibwebsockets::Event & e ){
    float lat, lon;
    string name;
    ofBuffer binary;
    
    parseWeatherEvent( e, binary, lat, lon, name );
    
    addConnection( e.conn, lat, lon );
    
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

//--------------------------------------------------------------
int FileManager::getLookupLat( float lat ){
    return ofMap(lat, -90, 90, 0, 3600, true);
}

//--------------------------------------------------------------
int FileManager::getLookupLong( float lon ){
    return ofMap(lon, -180, 180, 0, 1800, true);
}
//--------------------------------------------------------------
vector<string> & FileManager::getFiles( float lat, float lon ){
    int lookupLat = getLookupLat(lat);
    int lookupLon = getLookupLong(lon);
    
    static vector<string> emptyList;
    
    if ( latLongMap.count(lookupLat) == 0 || latLongMap[lookupLat].count(lookupLon == 0) ){
        return emptyList;
    } else {
        return latLongMap[lookupLat][lookupLon];
    }
}

//--------------------------------------------------------------
void FileManager::addConnection( ofxLibwebsockets::Connection & c, float lat, float lon ){
    int lookupLat = getLookupLat(lat);
    int lookupLon = getLookupLong(lon);
    
    // create vectors if they don't exist
    if ( latLongClientMap.count(lookupLat) == 0){
        latLongClientMap[lookupLat] = map<int, vector<ofxLibwebsockets::Connection*> >();
    }
    if( latLongClientMap[lookupLat].count(lookupLon == 0) ){
        latLongClientMap[lookupLat][lookupLon] = vector<ofxLibwebsockets::Connection*>();
    }
    
    latLongClientMap[lookupLat][lookupLon].push_back(&( c));
}

//--------------------------------------------------------------
vector<ofxLibwebsockets::Connection* > & FileManager::getConnections( float lat, float lon ){
    int lookupLat = getLookupLat(lat);
    int lookupLon = getLookupLong(lon);
    
    static vector<ofxLibwebsockets::Connection*> emptyList;
    
    if ( latLongClientMap.count(lookupLat) == 0 || latLongClientMap[lookupLat].count(lookupLon == 0) ){
        return emptyList;
    } else {
        return latLongClientMap[lookupLat][lookupLon];
    }
}

//--------------------------------------------------------------
void FileManager::removeConnection( ofxLibwebsockets::Connection & c ){
    map< int, map<int, vector<ofxLibwebsockets::Connection* > > >::iterator latIt;
    map<int, vector<ofxLibwebsockets::Connection* > >::iterator longIt;
    vector<ofxLibwebsockets::Connection* >::iterator connIt;
    
    for ( latIt = latLongClientMap.begin(); latIt != latLongClientMap.end(); ++latIt ){
        for ( longIt = latIt->second.begin(); longIt != latIt->second.end(); ++longIt ){
            for ( connIt = longIt->second.begin(); connIt != longIt->second.end(); ++connIt ){
                if (*connIt == &c ){
                    vector<ofxLibwebsockets::Connection* >::iterator del(connIt);
                    connIt++;
                    longIt->second.erase(del);
                }
            }
        }
    }
}
