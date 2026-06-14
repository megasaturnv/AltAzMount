#pragma once
#ifndef CLASS_ALTAZPREFERENCES_H
#define CLASS_ALTAZPREFERENCES_H

#include <Arduino.h>
#include <Preferences.h>

class class_AltAzPreferences {
private:
    Preferences preferences; // Instance of the ESP32 Preferences library
    const char* namespaceName; // Limited to 15 characters max by ESP32 NVS

public:
    // Constructor
    class_AltAzPreferences(const char* ns = "AltAz_config");

    // Lifecycle methods
    bool begin(bool readOnly = false);
    void end();
    void clear();
    void factoryReset();
    
    //Get and set latitude functions
    double getLatitude();
    void setLatitude(double value);
    
    //Get and set longitude functions
    double getLongitude();
    void setLongitude(double value);
};

#endif // CLASS_ALTAZPREFERENCES_H
