//
//  WUndergroundAPI.h
//  WeatherServer
//
//  Created by Brett Renfer on 6/12/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

// Poco stuff
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/StreamCopier.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"

using namespace Poco::Net;
using namespace Poco;

static const string wu_api_prefix = "api.wunderground.com/api/";

enum WUAPIFormat {
    FORMAT_XML = 0,
    FORMAT_JSON
};

class WUndergroundResponse {
public:
    string api_url;
    string results_json;
    ofxXmlSettings results_xml;
    int response_id;
    WUAPIFormat format;
    
    bool complete;
    void * user_data;
};

class WUndergroundAPI : protected ofThread {
public:
    
    ~WUndergroundAPI(){
        ofLogWarning() << "API closing down, waiting for thread";
        waitForThread();
        stopThread();
    }
    
    void setup( string _api_key, int call_rate = 6000 ){
        api_key = _api_key;
        api_call_rate = call_rate;
        response_id = 0;
        last_api_call = ofGetElapsedTimeMillis();
    }
    
    /**
     * Query conditions by some sort of geolocation: State/City, ZIP, contry/city, airport code
     * see: http://www.wunderground.com/weather/api/d/docs?d=data/index&MR=1
     */
    
    WUndergroundResponse getConditionsByGeo( string geo, WUAPIFormat format = FORMAT_XML ){
        return makeCall( "geolookup/conditions", "", geo );
    }
    
    /**
     * Query conditions by latitude + longitude
     */
    
    WUndergroundResponse getConditionsLatLong( float lat, float lon, WUAPIFormat format = FORMAT_XML ){
        return makeCall( "conditions", "", ofToString(lat) +","+ofToString(lon) );
    }
    
    /** Raw API call **/
    WUndergroundResponse makeCall( string features, string settings, string query, WUAPIFormat format = FORMAT_XML){
        vector<string> wu_features;
        vector<string> wu_settings;
        
        wu_features.push_back(features);
        if ( settings != "") wu_settings.push_back(settings);
        return makeCall( wu_features, wu_settings, query, format );
    }
    
    WUndergroundResponse makeCall( vector<string> features, vector<string> settings, string query, WUAPIFormat format = FORMAT_XML){
        response_id++;
        
        WUndergroundResponse req;
        req.format = format;
        req.response_id = response_id;
        req.api_url = buildAPICall(features, settings, query, format);
        lock();
        requestQueue.push_back(req);
        unlock();
        
        if (!isThreadRunning()){
            startThread();
        }
        
        return req;
    }
    
    ofEvent<WUndergroundResponse> onNewResponse;
    
protected:
    
    void threadedFunction(){
        while( isThreadRunning() ){
            if ( ofGetElapsedTimeMillis() - last_api_call > api_call_rate ){
                last_api_call = ofGetElapsedTimeMillis();
                
                WUndergroundResponse & current = requestQueue.front();
                
                if ( current.complete ){
                    int index = 1;
                    while (index < requestQueue.size()-1){
                        current = requestQueue.at(index);
                        if (!current.complete) break;
                        index++;
                    }
                }
                
                if ( !current.complete ){
                    
                    string result;
                    bool bSuccess = false;
                    
                    try
                    {
                        cout << "http://" + wu_api_prefix + current.api_url << endl;
                        ofHttpResponse resp = ofLoadURL("http://" + wu_api_prefix + current.api_url);
                        result = (string) resp.data;
                        bSuccess = true;
                        
                    }
                    catch (Exception &ex)
                    {
                        ofLogError() << ex.displayText();
                        bSuccess = false;
                    }
                    
                    if ( bSuccess ){
                        if ( current.format == FORMAT_XML){
                            bool bLoaded = current.results_xml.loadFromBuffer(result);
                            cout << "loaded xml? "<<bLoaded<<endl;
                        } else {
                            current.results_json = result;
                        }
                        ofNotifyEvent(onNewResponse, current, this);
                    }
                    current.complete = true;
                } else {
                    // done for now.
                    stopThread();
                }
            }
            sleep(100);
        }
    }
    
private:
    vector<WUndergroundResponse> requestQueue;
    int     last_api_call, api_call_rate;
    int     response_id;
    string api_key;
    
    string buildAPICall( vector<string> features, vector<string> settings, string query, WUAPIFormat format = FORMAT_XML ){
        string ret = api_key + "/";
        
        for ( auto s : features ){
            ret += s + "/";
        }
        
        for ( auto s : settings ){
            ret += s + "/";
        }
        
        ret += "q/" + query + "." + (format == FORMAT_JSON ? "json" : "xml");
        
        return ret;
    }
};
