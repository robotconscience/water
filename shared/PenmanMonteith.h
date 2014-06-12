//
//  PenmanMonteith.h
//  WeatherServer
//
//  C++ port of https://github.com/allanglen/evapotrans-php

#pragma once
#include <iostream.h>
#include <stdint.h>
#include <stdio.h>      
#include <time.h>
#include <math.h>

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (M_PI/180.0)
#endif

class PenmanMonteith
{
public:
    
    PenmanMonteith();
    
    /*
     * Description:
     *  Calculate the evapotranspiration for a 24 hour period 
     *  at a specified latitude
     *
     * Parameters:
     *  latitude - Latitude in degrees
     *  temperatureMinF - Minimum temperature in farenheit
     *  temperatureMaxF - Maximum temperature in farenheit
     *  relativeHumidityMin - Minimum relative humidity (percent)
     *  relativeHumidityMax - Maximum relative humidity (percent)
     *  windspeedMilesPerHour - Wind speed in miles per hour
     *
     * Returns:
     *  Estimated evapotranspiration in inches rounded to 3 decimal places.
     */
    float getEvapotranspiration( float latitude,
                                 float temperatureMinF, float temperatureMaxF,
                                 float relativeHumidityMin, float relativeHumidityMax,
                                 float windspeedMilesPerHour);
    
    
    // The elevation of the weather station
    float elevationFeet;
    
    // The albedo of the surface.  The default albedo (0.23) is based on a reference crop of 0.12 meters
    // and is commonly used for short grass surfaces
    float albedo;
    
private:
    
    /*
     * Description:
     *  Calculate the evapotranspiration for a 24 hour period.
     *
     * Parameters:
     *  temperatureMinF - Minimum temperature in farenheit
     *  temperatureMaxF - Maximum temperature in farenheit
     *  relativeHumidityMin - Minimum relative humidity (percent, 0-100)
     *  relativeHumidityMax - Maximum relative humidity (percent, 0-100)
     *  windspeedMilesPerHour - Wind speed in miles per hour
     *
     * Returns:
     *  Estimated evapotranspiration in inches rounded to 3 decimal places.
     */
    float getEvapotranspiration(float temperatureMinF, float temperatureMaxF,
                                float relativeHumidityMin, float relativeHumidityMax,
                                float windspeedMilesPerHour);
    
    // The latitude of the weather station in degrees
    float latitude;
    
    /*
     * Description:
     *  Calculate the estimated evapotranspiration in millimeters for a 24 hour period.
     *
     * Parameters:
     *  slopeOfSaturationVapourPressureCurve - Slope of the saturation vapour pressure curve
     *  netRadiation - Net radiation for the 24 hour period
     *  psychrometricConstant - The psychrometric constant
     *  temperatureMeanC - Mean daily temperature in celcius
     *  windspeedMetersPerSecond - Wind speed in meters per second
     *  vapourPressureDeficit - Vapour pressure defecit
     *
     * Returns:
     *  Estimated evapotranspiration in millimeters.
     */
    float penmanMoneteithEvapotranspiration( float slopeOfSaturationVapourPressureCurve,
                                            float netRadiation,
                                            float psychrometricConstant,
                                            float temperatureMeanC,
                                            float windspeedMetersPerSecond,
                                            float vapourPressureDeficit);
    
    /*
     * Description:
     *  Convert farenheit to celcius.
     *
     * Parameters:
     *  farenheit - Temperature in farenheit
     *
     * Returns:
     *  Temperature in celcius.
     */
    float farenheitToCelcius(float farenheit);
    
    /*
     * Description:
     *  Convert celcius to kelvin.
     *
     * Parameters:
     *  celcius - Temperature in celcius
     *
     * Returns:
     *  Temperature in kelvin.
     */
    float celciusToKelvin(float celcius);
    
    /*
     * Description:
     *  Convert feet to meters.
     *
     * Parameters:
     *  feet - Distance in feet
     *
     * Returns:
     *  Distance in meters.
     */
    float feetToMeters(float feet);
    
    /*
     * Description:
     *  Convert miles per hour to meters per second.
     *
     * Parameters:
     *  milesPerHour - Miles per hours
     *
     * Returns:
     *  Speed in meters per second.
     */
    float milesPerHourToMetersPerSecond(float milesPerHour);
    
    /*
     * Description:
     *  Convert millimeters to inches.
     *
     * Parameters:
     *  mm - Millimeters
     *
     * Returns:
     *  Distance in inches.
     */
    float mmToIn(float mm);
    
    
    /*
     * Description:
     *  Returns the current day of the year based on the system clock.
     *
     * Returns:
     *  Day of the year where 1 is the first day of the year.
     */
    int dayOfYear();
    
    /*
     * Description:
     *  Calculates the estimated atmospheric pressure at a specific elevation based on the ideal gas law at 20 degrees C.
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#atmospheric parameters
     *
     * Parameters:
     *  elevationMeters - Elevation in meters
     *
     * Returns:
     *  Atmospheric pressure (kPa)
     */
    // Atmospheric pressure (P)
    float getAtmosphericPressure(float elevationMeters);
    
    /*
     * Description:
     *  Calculates the psychrometric constant atmospheric pressure at a specific elevation based on the ideal gas law at 20 degrees C.
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#psychrometric constant (g)
     *
     * Parameters:
     *  atmosphericPressure - Atmospheric pressure in kPa
     *
     * Returns:
     *  Psychrometric constant (kPa �C-1)
     */
    float  getPsychrometricConstant(float atmosphericPressure);
    
    /*
     * Description:
     *  Calculates the saturation vapour pressure at a specified temperature
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#air humidity
     *
     * Parameters:
     *  temperatureC - Temperature in celcius
     *
     * Returns:
     *  Saturation vapour pressure (kPa)
     */
    float  getSaturationVapourPressure(float temperatureC);
    
    /*
     * Description:
     *  Calculate the mean (average) of two values)
     *
     * Parameters:
     *  value1 - The first value
     *  value2 - The second value
     *
     * Returns:
     *  The mean of the two parameters
     */
    float mean(float value1, float value2);
    
    /*
     * Description:
     *  Calculates the slope of the saturation vapour pressure curve at a specified temperature
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#air humidity
     *
     * Parameters:
     *  temperatureC - Temperature in celcius
     *
     * Returns:
     *  Slope of the saturation vapour pressure curve (kPa �C-1)
     */
    float getSlopeOfSaturationVapourPressureCurve(float temperatureC);
    
    /*
     * Description:
     *  Calculates the actual vapour pressure
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#air humidity
     *
     * Parameters:
     *  saturationVapourPressureMin - Minimum saturation vapour pressure (kPa)
     *  saturationVapourPressureMax - Maximum saturation vapour pressure (kPa)
     *  relativeHumidityMin - Minimum relative humidity (%)
     *  relativeHumidityMax - Masimum relative humidity (%)
     *
     * Returns:
     *  Actual vapour pressure (kPa)
     */
    float getActualVapourPressure(float saturationVapourPressureMin,float saturationVapourPressureMax,
                               float relativeHumidityMin,float relativeHumidityMax);
    
    /*
     * Description:
     *  Calculates the vapour pressure deficit
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#air humidity
     *
     * Parameters:
     *  saturationVapourPressureMin - Minimum saturation vapour pressure (kPa)
     *  saturationVapourPressureMax - Maximum saturation vapour pressure (kPa)
     *  actualVapourPressure - Actual vapour pressure (kPa)
     *
     * Returns:
     *  Vapour pressure deficit (kPa)
     */
    float getVapourPressureDeficit(float saturationVapourPressureMin,
                                float saturationVapourPressureMax,
                                float actualVapourPressure);
    
    /*
     * Description:
     *  Calculates the daily extraterrestrial radiation at a specific latitude at a specific time of year
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  inverseRelativeDistanceEarthSun - Inverse relative distance Earth-Sun
     *  sunsetHourAngle - Sunset hour angle (radians)
     *  latitude - Latitude (degrees)
     *  solarDeclination - Solar declination (radians)
     *
     * Returns:
     *  Extraterrestrial radiation (MJ m-2 day-1)
     */
    float getExtraterrestrialRadiationDaily(float inverseRelativeDistanceEarthSun, float sunsetHourAngle, float latitude, float solarDeclination);
    
    /*
     * Description:
     *  Calculates the inverse relative distance earth-sun based on the day of the year.
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  dayOfYear - Day of the year where 1 is the first day of the year
     *
     * Returns:
     *  Inverse relative distance earth-sun (radians)
     */
    float getInverseRelativeDistanceEarthSun(int dayOfYear);
    
    /*
     * Description:
     *  Calculates the solar declination based on the day of the year.
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  dayOfYear - Day of the year where 1 is the first day of the year
     *
     * Returns:
     *  Solar declination (radians)
     */
    float getSolarDeclination(int dayOfYear);
    
    /*
     * Description:
     *  Calculates the sunset hour angle based on the latitude and the solar declination
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  latitude - Latitude (degrees)
     *  solarDeclination - Solar declination (radians)
     *
     * Returns:
     *  Sunset hour angle (radians)
     */
    float getSunsetHourAngle(float latitude,float solarDeclination);
    
    /*
     * Description:
     *  Calculates the daylight hours given the sunset hour angle
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  sunsetHourAngle - Sunset hour angle (radians)
     *
     * Returns:
     *  Daylight hours
     */
    float getDaylightHours(float sunsetHourAngle);
    
    /*
     * Description:
     *  Calculates the solar radiation
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  sunshineHoursActual - Actual sunshine hours
     *  sunshineHoursMax - Maximum sunshine hours
     *  extraterrestrialRadiationDaily - Daily extraterrestrial radiation (MJ m-2 day-1)
     *
     * Returns:
     *  Daylight hours
     */
    float getSolarRadiation(float sunshineHoursActual,float sunshineHoursMax,float extraterrestrialRadiationDaily);
    
    /*
     * Description:
     *  Calculates the net solar radiation
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  albedo - Albedo of the surface
     *  incomingSolarRadiation - Incoming solar radiation (MJ m-2 day-1)
     *
     * Returns:
     *  Net solar radiation (MJ m-2 day-1)
     */
    float getNetSolarRadiation(float albedo,float incomingSolarRadiation);
    
    /*
     * Description:
     *  Calculates the net longwave radiation
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  temperatureMinC - Minimum temperature (C)
     *  temperatureMaxC - Maximum temperature (C)
     *  actualVapourPressure - Actual vapour pressure (kPa)
     *  incomingSolarRadiation - Incoming solar radiation (MJ m-2 day-1)
     *  clearSkySolarRadiation - Clear sky solar radiation (MJ m-2 day-1)
     *
     * Returns:
     *  Net longwave radiation (MJ m-2 day-1)
     */
    float getNetLongwaveRadiation(float temperatureMinC,float temperatureMaxC,
                               float actualVapourPressure,float incomingSolarRadiation,float clearSkySolarRadiation);
    
    /*
     * Description:
     *  Calculates the net radiation
     *  See http://www.fao.org/docrep/X0490E/x0490e07.htm#radiation
     *
     * Parameters:
     *  netSolarRadiation - Net solar radiation (MJ m-2 day-1)
     *  netLongwaveRadiation - Net longwave radiation (MJ m-2 day-1)
     *
     * Returns:
     *  Net radiation (MJ m-2 day-1)
     */
    float getNetRadiation(float netSolarRadiation,float netLongwaveRadiation);
    
};


