#include "class_AltAzPreferences.h"
#include "settings.h"
#include "global_variables.h"
#include "functions_oled_print.h"

// Initialize with a specified namespace name
class_AltAzPreferences::class_AltAzPreferences(const char* ns) {
    namespaceName = ns;
}

// Opens the NVS storage namespace
bool class_AltAzPreferences::begin(bool readOnly) {
    return preferences.begin(namespaceName, readOnly);
}

// Closes the NVS namespace to free up memory
void class_AltAzPreferences::end() {
    preferences.end();
}

// Clears all parameters within the active namespace
void class_AltAzPreferences::clear() {
    if (begin(false)) {
        preferences.clear();
        end();
    }
}

// Wipes custom data AND restores safe factory defaults immediately
void class_AltAzPreferences::factoryReset() {
    // 1. Wipe the current namespace completely blank
    clear();

    // 2. Immediately commit safe, out-of-the-box factory configurations
    //setBootCount(0);
    //setDeviceName("ESP32-Default-Factory");
    setLatitude(defaultPreference_latitude);
    setLongitude(defaultPreference_longitude);

    // Optional: Log the event
    OLED_print("Defaults restored", INFO);
}

//Get and set latitude functions
double class_AltAzPreferences::getLatitude() {
    double value = 0.0;
    if (begin(true)) { // Open in Read-Only mode to protect flash from accidental writes
        value = preferences.getDouble("latitude", defaultPreference_latitude);
        end();
    }
    return value;
}

void class_AltAzPreferences::setLatitude(double value) {
    // Only write if the value is different from what is already stored
    if (getLatitude() != value) {
        if (begin(false)) {
            preferences.putDouble("latitude", value);
            end();
        }
    }
}

//Get and set longitude functions
double class_AltAzPreferences::getLongitude() {
    double value = 0.0;
    if (begin(true)) { // Open in Read-Only mode to protect flash from accidental writes
        value = preferences.getDouble("longitude", defaultPreference_longitude);
        end();
    }
    return value;
}

void class_AltAzPreferences::setLongitude(double value) {
    // Only write if the value is different from what is already stored
    if (getLongitude() != value) {
        if (begin(false)) {
            preferences.putDouble("longitude", value);
            end();
        }
    }
}
