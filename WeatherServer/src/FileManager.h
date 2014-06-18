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

class FileManager {
public:
    
    vector<ofFile> getFiles( float lat, float lon ){
        // lookup table = -180 to 180   (long) mapped from 0 to 3600
        // lookup table = -90 to 90     (lat) mapped from 0 to 1800
        int lookupLat = ofMap(lat, -90, 90, 0, 3600, true);
        int lookupLon = ofMap(lat, -180, 180, 0, 1800, true);
        
        static vector<ofFile> emptyList;
        
        if ( latLongMap.count(lookupLat) == 0 || latLongMap[lookupLat].count(lookupLon == 0) ){
            return emptyList;
        } else {
            return latLongMap[lookupLat][lookupLon];
        }
    }
    
protected:
    
    map< int, map<int, vector<ofFile> > > latLongMap;
    
};