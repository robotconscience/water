//
//  PenmanMonteith.cpp
//  WeatherServer
//
//  Created by Brett Renfer on 6/12/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "PenmanMonteith.h"

// Solar radiation factor for overcast and cloudy
static float fractionOfRadiationOvercast = 0.25f;
static float fractionOfRadiationClear = 0.5f;

//--------------------------------------------------------------
PenmanMonteith::PenmanMonteith(){
    elevationFeet = 0;
    latitude = 0;
    albedo = 0.23;
}

//--------------------------------------------------------------
float PenmanMonteith::getEvapotranspiration( float lat, float temperatureMinF, float temperatureMaxF,
                                            float relativeHumidityMin, float relativeHumidityMax, float windspeedMilesPerHour)
{
    latitude = lat;
    return getEvapotranspiration(temperatureMinF, temperatureMaxF, relativeHumidityMin, relativeHumidityMax, windspeedMilesPerHour);
}

//--------------------------------------------------------------
float PenmanMonteith::getEvapotranspiration( float temperatureMinF, float temperatureMaxF,
                                             float relativeHumidityMin, float relativeHumidityMax, float windspeedMilesPerHour)
{
    
    // Get the day of the year
    int day = dayOfYear();
    
    // Convert input values to metric
    float elevationMeters = feetToMeters(elevationFeet);
    float temperatureMinC = farenheitToCelcius(temperatureMinF);
    float temperatureMaxC = farenheitToCelcius(temperatureMaxF);
    float temperatureMeanC = mean(temperatureMinC,temperatureMaxC);
    float windspeedMetersPerSecond = milesPerHourToMetersPerSecond(windspeedMilesPerHour);
    
    // Calculate meteorological parameters for Penman-Monteith equation
    float atmosphericPressure = getAtmosphericPressure(elevationMeters);
    
    float psychrometricConstant = getPsychrometricConstant(atmosphericPressure);
    
    float saturationVapourPressureMin = getSaturationVapourPressure(temperatureMinC);
    float saturationVapourPressureMax = getSaturationVapourPressure(temperatureMaxC);
    float saturationVapourPressureMean = mean(saturationVapourPressureMin,saturationVapourPressureMax);
    
    float slopeOfSaturationVapourPressureCurve = getSlopeOfSaturationVapourPressureCurve(saturationVapourPressureMean);
    
    float actualVapourPressure = getActualVapourPressure(saturationVapourPressureMin,
                                                      saturationVapourPressureMax,
                                                      relativeHumidityMin,
                                                      relativeHumidityMax);
    
    float vapourPressureDeficit = getVapourPressureDeficit(saturationVapourPressureMin,
                                                        saturationVapourPressureMax,
                                                        actualVapourPressure);
    
    // Calculate solar radiation paramters for Penman-Monteith equation
    float invRelativeDistanceEarthSun = getInverseRelativeDistanceEarthSun(day);
    
    float solarDeclination = getSolarDeclination(day);
    
    float sunsetHourAngle = getSunsetHourAngle(latitude,solarDeclination);
    
    float extraterrestrialRadiationDaily = getExtraterrestrialRadiationDaily(invRelativeDistanceEarthSun,
                                                                          sunsetHourAngle,
                                                                          latitude,
                                                                          solarDeclination);
    float daylightHours = getDaylightHours(sunsetHourAngle);
    
    float clearSkySolarRadiation = getSolarRadiation(daylightHours,daylightHours,
                                                  extraterrestrialRadiationDaily);
    
    // Note: Clear sky is assumed for the purposes of this estimate as actual sunshine hours are not typically available
    // The difference is not significant as changes in direct sunlight typically result in lower temperatures and
    // higher humidity.
    float actualSunshineHours = daylightHours;
    
    float incomingSolarRadiation = getSolarRadiation(actualSunshineHours,daylightHours,
                                                  extraterrestrialRadiationDaily);
    
    float netSolarRadiation = getNetSolarRadiation(albedo,incomingSolarRadiation);
    
    float netLongwaveRadiation = getNetLongwaveRadiation(temperatureMinC,
                                                      temperatureMaxC,
                                                      actualVapourPressure,
                                                      incomingSolarRadiation,
                                                      clearSkySolarRadiation);
    
    float netRadiation = getNetRadiation(netSolarRadiation,netLongwaveRadiation);
    
    // Calculation the evapotranspiration (metric)
    float evapotranspirationMm = penmanMoneteithEvapotranspiration(slopeOfSaturationVapourPressureCurve,
                                                                   netRadiation,
                                                                   psychrometricConstant,
                                                                   temperatureMeanC,
                                                                   windspeedMetersPerSecond,
                                                                   vapourPressureDeficit);
    
    // Convert evapotranpiration to inches
    return mmToIn(evapotranspirationMm);
}


//--------------------------------------------------------------
float PenmanMonteith::penmanMoneteithEvapotranspiration( float slopeOfSaturationVapourPressureCurve,
                                        float netRadiation,
                                        float psychrometricConstant,
                                        float temperatureMeanC,
                                        float windspeedMetersPerSecond,
                                        float vapourPressureDeficit)
{

    float numerator = (0.408 * slopeOfSaturationVapourPressureCurve * netRadiation) +
    (psychrometricConstant * (900 / (temperatureMeanC + 273)) * windspeedMetersPerSecond * vapourPressureDeficit);
    
    float denominator = slopeOfSaturationVapourPressureCurve + (psychrometricConstant * (1 + (0.34 * windspeedMetersPerSecond)));
    
    return numerator / denominator;
}

float PenmanMonteith::farenheitToCelcius(float farenheit) {
    return (farenheit - 32.0) * (5.0/9.0);
}

float PenmanMonteith::celciusToKelvin(float celcius) {
    return celcius + 273.16;
}

float PenmanMonteith::feetToMeters(float feet) {
    return feet * 0.3048;
}

float PenmanMonteith::milesPerHourToMetersPerSecond(float milesPerHour) {
    return milesPerHour * 0.44704;
}

float PenmanMonteith::mmToIn(float mm) {
    return mm * 0.0393700787;
}

int PenmanMonteith::dayOfYear() {
    time_t    curr;
    tm       local;
    time(&curr);
    local   =*(localtime(&curr));
    
    return local.tm_mday + 1;
}

float PenmanMonteith::getAtmosphericPressure(float elevationMeters) {
    return 101.3 * pow(((293 - (0.0065 * elevationMeters))/293),5.26);
}

float PenmanMonteith::getPsychrometricConstant(float atmosphericPressure)
{
    return 0.000665 * atmosphericPressure;
}

float PenmanMonteith::getSaturationVapourPressure(float temperatureC) {
    return 0.6108 * exp((17.27 * temperatureC) / (temperatureC + 237.3));
}

float PenmanMonteith::mean(float value1, float value2) {
    return (value1 + value2) / 2.0;
}

float PenmanMonteith::getSlopeOfSaturationVapourPressureCurve(float temperatureC) {
    return (4098 * getSaturationVapourPressure(temperatureC)) / pow((temperatureC + 237.3),2);
}

float PenmanMonteith::getActualVapourPressure(float saturationVapourPressureMin,float saturationVapourPressureMax,
                           float relativeHumidityMin,float relativeHumidityMax) {
    return mean(saturationVapourPressureMin * (relativeHumidityMax / 100),
                saturationVapourPressureMax * (relativeHumidityMin / 100));
}

float PenmanMonteith::getVapourPressureDeficit(float saturationVapourPressureMin,float saturationVapourPressureMax,
                            float actualVapourPressure) {
    return mean(saturationVapourPressureMin,saturationVapourPressureMax) - actualVapourPressure;
}

float PenmanMonteith::getExtraterrestrialRadiationDaily(float inverseRelativeDistanceEarthSun, float sunsetHourAngle, float latitude, float solarDeclination) {
    float solarConstant = 0.0820;
    
    float angleCalculation = (sunsetHourAngle * sin(latitude * DEG_TO_RAD) * sin(solarDeclination)) +
    (cos(latitude * DEG_TO_RAD) * cos(solarDeclination) * sin(sunsetHourAngle));
    
    return ((24 * 60) / M_PI) * solarConstant * inverseRelativeDistanceEarthSun * ( angleCalculation );
}

float PenmanMonteith::getInverseRelativeDistanceEarthSun(int dayOfYear) {
    return 1.0 + (0.033 * cos(((2.0 * M_PI) / 365.0) * (float) dayOfYear));
}

float PenmanMonteith::getSolarDeclination(int dayOfYear) {
    return 0.409 * sin((((2.0 * M_PI)/365.0)*dayOfYear)-1.39);
}

float PenmanMonteith::getSunsetHourAngle(float latitude,float solarDeclination) {
    return acos(-tan(latitude * DEG_TO_RAD) * tan(solarDeclination));
}

float PenmanMonteith::getDaylightHours(float sunsetHourAngle) {
    return (24.0 / M_PI) * sunsetHourAngle;
}

float PenmanMonteith::getSolarRadiation(float sunshineHoursActual,float sunshineHoursMax,float extraterrestrialRadiationDaily) {
    
    fractionOfRadiationOvercast = fractionOfRadiationOvercast;
    fractionOfRadiationClear = fractionOfRadiationClear;
    
    return (fractionOfRadiationOvercast + (fractionOfRadiationClear*(sunshineHoursActual / sunshineHoursMax))) *
    extraterrestrialRadiationDaily;
}

float PenmanMonteith::getNetSolarRadiation(float albedo,float incomingSolarRadiation) {
    return (1 - albedo) * incomingSolarRadiation;
}

float PenmanMonteith::getNetLongwaveRadiation(float temperatureMinC,float temperatureMaxC,
                           float actualVapourPressure,float incomingSolarRadiation,float clearSkySolarRadiation) {
    
    // Convert temperature to kelvin
    float temperatureMinK = celciusToKelvin(temperatureMinC);
    float temperatureMaxK = celciusToKelvin(temperatureMaxC);
    
    // Calculate the average temperature to the 4th power
    float temperatureAvgFourthPower = (pow(temperatureMinK,4) + pow(temperatureMaxK,4)) / 2;
    
    // Calculate the relative shortwave radiation
    float relativeShortwaveRadiation = incomingSolarRadiation / clearSkySolarRadiation;
    
    // Calculate the net longwave radiation
    return 0.000000004903 * temperatureAvgFourthPower *
    (0.34 - (0.14 * sqrt(actualVapourPressure))) * ((1.35 * relativeShortwaveRadiation) - 0.35);
}

float PenmanMonteith::getNetRadiation(float netSolarRadiation,float netLongwaveRadiation) {
    return netSolarRadiation - netLongwaveRadiation;
}