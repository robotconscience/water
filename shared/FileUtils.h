//
//  FileUtils.h
//  WeatherServer
//
//  Created by Brett Renfer on 6/18/14.
//
//

#pragma once
// method to parse JSON out of front of message
static void parseWeatherEvent( ofxLibwebsockets::Event & event, ofBuffer & out_buff, float & out_lat, float & out_lon, string & out_name ){
    unsigned long jsonLength = event.data.getBinaryBuffer()[0];
    int jsonStartIndex = 1;
    if (jsonLength == 254){
        if (event.data.size() > 3){
            jsonLength = event.data.getBinaryBuffer()[1];
            jsonStartIndex = 3;
        } else {
            ofLogError()<<"[FileUtils] Binary message of incorrect format";
            return;
        }
    } else if (jsonLength == 255){
        if (event.data.size() > 5){
            jsonLength = event.data.getBinaryBuffer()[1];
            jsonStartIndex = 5;
        } else {
            ofLogError()<<"[FileUtils] Binary message of incorrect format";
            return;
        }
    }
    
    if (jsonLength > 0 ){
        string jsonStr = event.data.getText().substr(jsonStartIndex, jsonLength);
        
        static Json::Reader jsonReader;
        Json::Value json;
        jsonReader.parse(jsonStr, json);
        
        out_name = json["name"].asString();
        out_lat = ofToFloat(json["lat"].asString());
        out_lon = ofToFloat(json["long"].asString());
        
        // value == size of binary data
        stringstream s; s<<json["size"];
        string value = s.str();
        
        int size = ofToInt(value);
        char * data = (char*) calloc(size, sizeof(char) );
        memcpy(data, event.data.getBinaryBuffer() + jsonStartIndex + jsonLength, size);
        
        cout << size << endl;
        
        out_buff.set(data, size);
        
    } else {
        ofLogError() << "[FileUtils] got bad binary frame (no json)";
    }
}

static string getJSON( string app_name, float lat, float lon, int size ){
    stringstream ss;
    ss<<"{\"name\":\""<<app_name<<"\",";
    ss<<"\"lat\":\""<<lat<<"\",";
    ss<<"\"long\":\""<<lon<<"\",";
    ss<<"\"size\":"<<size<<"}";
    return ss.str();
}

static void getSendBuffer( ofBuffer & in_buffer, ofBuffer & out_buffer, string app_name, float lat, float lon ){
    out_buffer.clear();
    string json = getJSON(app_name, lat, lon, in_buffer.size() );
    unsigned int jsonByteLength = json.length();
    int numBytesForJsonLength = (jsonByteLength > 0xFFFF ? 5 : (jsonByteLength >= 254 ? 3 : 1));
    if (numBytesForJsonLength == 5){
        char c[] = {255};
        out_buffer.append(c, 1);
        char len[] = {jsonByteLength >> 24, jsonByteLength >> 16, jsonByteLength >> 8, jsonByteLength};
        out_buffer.append(len, 4);
    } else if (numBytesForJsonLength == 3){
        char c[] = {254};
        out_buffer.append(c, 1);
        char len[] = {jsonByteLength >> 8, jsonByteLength};
        out_buffer.append(len, 2);
    } else {
        char len[] = {jsonByteLength};
        out_buffer.append(len, 1);
    }
    out_buffer.append( json );
    out_buffer.append( in_buffer.getBinaryBuffer(), in_buffer.size() );
}