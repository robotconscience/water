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

struct EvapPrecipResponse
{
    int response_id; // tied to WUndergroudAPI call
    float evapotranspiration;
    float precipitation;
    float lat, lon;
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
    
    /**
     * OK, it's actually evapotranspiration...
     */
    int lookupEvaporation( float lat, float lon ){
        manipulateLatLong( lat, lon );
        
        // first: have we looked this up recently?
        string ll = ofToString(lat)+":"+ofToString(lon);
        bool bFound = false;
        if ( currentConditions.count(ll) > 0 ){
            bFound = true;
            WeatherConditions curr = currentConditions[ll];
            int time = ofGetElapsedTimeMillis() - curr.lastUpdated;
            if( time < refreshRate){
                //TODO: NOTIFY EVENT WITH CURRENT PRECIP!
                return;
            }
        }
        
        // new request
        if ( !bFound ){
            currentConditions[ll] = WeatherConditions();
            currentConditions[ll].lat = lat;
            currentConditions[ll].lon = lon;
        }
        currentConditions[ll].lastUpdated = ofGetElapsedTimeMillis();
        
        // make API call
        WUndergroundResponse req = api.getConditionsLatLong(lat, lon);
        
        // store in outgoing
        outgoingResponses.push_back( EvapPrecipResponse() );
        outgoingResponses.back().response_id = req.response_id;
        outgoingResponses.back().lat = lat;
        outgoingResponses.back().lon = lon; // should these be the unmanipulated ones?
        
        return req.response_id;
    }
    
    void onAPIResponse( WUndergroundResponse & response ){
        // find outgoing response
        EvapPrecipResponse outgoing;
        outgoing.lat = -1000;
        
        for ( int i=0; i<outgoingResponses.size(); i++ ){
            if ( outgoingResponses[i].response_id == response.response_id ){
                outgoing = outgoingResponses[i]; // copy that shit
                outgoingResponses.erase(outgoingResponses.begin() + i );
                break;
            }
        }
        
        if ( outgoing.lat == -1000 ){
            ofLogError()<<"[WeatherManager] Something went wrong!";
        }
        
        string read;
        response.results_xml.copyXmlToString(read);
        
        ofxXmlSettings xml;
        xml.loadFromBuffer(read);
        
        xml.pushTag("response");
        xml.pushTag("current_observation");
        
        float tempMin = xml.getValue("temp_f", 0.0);
        float tempMax = xml.getValue("temp_f", 0.0);
        float precipToday = xml.getValue("precip_today_in", 0.0);
        string humid_str = xml.getValue("relative_humidity", "0");
        humid_str = humid_str.substr(0, humid_str.length()-1); // trim off %
        float humidMin = ofToFloat(humid_str);
        float humidMax = ofToFloat(humid_str);
        float windSpeed = xml.getValue("wind_mph", 0.0);
        
        xml.popTag();
        xml.popTag();
        
        // return evapotrans
        outgoing.precipitation = precipToday;
        outgoing.evapotranspiration = penman.getEvapotranspiration(outgoing.lat, tempMin, tempMax, humidMin, humidMax, windSpeed);
        ofNotifyEvent(onEvaporationLookup, outgoing, this);
    }
    
    ofEvent<EvapPrecipResponse> onEvaporationLookup;
    
private:
    int refreshRate;
    map<string, WeatherConditions>  currentConditions;
    vector<EvapPrecipResponse>      outgoingResponses;
    WUndergroundAPI api;
    PenmanMonteith penman;
    
    void manipulateLatLong( float & lat, float & lon ){
        // nothing for now
    }
};
