//
//  FileClient
//  WeatherClient
//
//  Created by Brett Renfer on 6/18/14.
//
//

#pragma once

#include "ofxLibwebsockets.h"
#include "FileUtils.h"

class FileClient {
public:
    
    void send( string filename, string client_name, float lat, float lon, ofxLibwebsockets::Client & client ){
        ofFile file( filename );
        fileBuffer = file.readToBuffer();
        file.remove();
        
        // can call as void since just writing to outputBuffer
        getSendBuffer( fileBuffer, outputBuffer, client_name, lat, lon);
        client.sendBinary( outputBuffer );
    }
    
    // got a file: parse + save
    void receive( ofxLibwebsockets::Event & e ){
        float lat, lon;
        string name;
        ofBuffer binary;
        
        parseWeatherEvent( e, binary, lat, lon, name );
        
        ofFile file;
        file.open( "files/" + ofGetTimestampString() + ".jpg", ofFile::WriteOnly );
        file.writeFromBuffer(binary);
        file.close();
    }
    
protected:
    
    ofBuffer outputBuffer, fileBuffer;
    
};