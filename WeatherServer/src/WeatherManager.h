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

class WeatherManager
{
public:
    
    /**
     * OK, it's actually evapotranspiration...
     */
    float getEvaporation( float lat, float lon ){
        // lookup temp, humidity, windspeed
        float tempMin, tempMax, humidMin, humidMax, windMPH;
        // to-do
        
        // return evapotrans
        return penman.getEvapotranspiration(lat, tempMin, tempMax, humidMin, humidMax, windMPH);
    }
    
private:
    
    PenmanMonteith penman;
};
