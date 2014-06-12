//
//  WeatherManager.h
//  WeatherServer
//
//  Created by Brett Renfer on 6/12/14.
//
//  Evapotranspiration algo ported from this PHP implementation:
//  https://raw.githubusercontent.com/allanglen/evapotrans-php/master/penmanmonteith.php


#pragma once

#include "PenmanMonteith.h"
#include "WUndergroundAPI.h"

struct WeatherConditions
{
    /* degrees */
    float lat, lon;
    /* farenheit */
    float tempMin, tempMax;
    /* percent 0-100 */
    float humidMin, humidMax;
    /* mph */
    float windSpeed;
    /* millis */
    int lastUpdated;
};

class WeatherManager
{
public:
    
    void setup(){
        // load API key
        ofFile key("api_key.txt"); ofBuffer buff = key.readToBuffer();
        string api_key = buff.getFirstLine();
        api.setup(api_key);
        refreshRate = 15000; // only update every 15 seconds (longer?)
        
        ofAddListener(api.onNewResponse, this, &WeatherManager::onAPIResponse);
    }
    
    float latitude;
    
    /**
     * OK, it's actually evapotranspiration...
     */
    void lookupEvaporation( float lat, float lon ){
        latitude = lat;
        // first: have we looked this up recently?
        string ll = ofToString(lat)+":"+ofToString(lon);
        bool bFound = false;
        if ( currentConditions.count(ll) > 0 ){
            bFound = true;
            WeatherConditions curr = currentConditions[ll];
            int time = ofGetElapsedTimeMillis() - curr.lastUpdated;
            if( time < refreshRate){
                return;
            }
        }
        
        if ( !bFound ){
            currentConditions[ll] = WeatherConditions();
            currentConditions[ll].lat = lat;
            currentConditions[ll].lon = lon;
        }
        currentConditions[ll].lastUpdated = ofGetElapsedTimeMillis();
        
        WUndergroundResponse & req = api.getConditionsLatLong(lat, lon);
        req.user_data = (void *) &currentConditions[ll];
        // to-do: do we need to keep track of these requests?
    }
    
    void onAPIResponse( WUndergroundResponse & response ){
        //WeatherConditions * curr = (WeatherConditions*) response.user_data;
        string read;
        response.results_xml.copyXmlToString(read);
        
        ofxXmlSettings xml;
        xml.loadFromBuffer(read);
        
        xml.pushTag("response");
        xml.pushTag("current_observation");
        
        float tempMin = xml.getValue("temp_f", 0.0);
        float tempMax = xml.getValue("temp_f", 0.0);
        string humid_str = xml.getValue("relative_humidity", "0");
        humid_str = humid_str.substr(0, humid_str.length()-1); // trim off %
        float humidMin = ofToFloat(humid_str);
        float humidMax = ofToFloat(humid_str);
        float windSpeed = xml.getValue("wind_mph", 0.0);
        
        xml.popTag();
        xml.popTag();
        
        // return evapotrans
        float trans = penman.getEvapotranspiration(latitude, tempMin, tempMax, humidMin, humidMax, windSpeed);
        ofNotifyEvent(onEvaporationLookup, trans, this);
        cout << trans << endl;
    }
    
    ofEvent<float> onEvaporationLookup;
    
private:
    int refreshRate;
    map<string, WeatherConditions> currentConditions;
    WUndergroundAPI api;
    PenmanMonteith penman;
};
