#include "functions_AltAzPreferences.h"

#include "global_variables.h"

// #####################################################
// # Functions for using AltAzPreferences custom class #
// #####################################################
// Load preferences from "eeprom" (flash memory on ESP32) into global variables
void LoadPreferencesIntoMemory() {
  latitudeDecimal = altAzPreferences.getLatitude(); 
  longitudeDecimal = altAzPreferences.getLongitude(); 
}
