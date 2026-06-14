#pragma once
#ifndef FUNCTIONS_ALTAZPREFERENCES_H
#define FUNCTIONS_ALTAZPREFERENCES_H

#include "global_variables.h"

// #####################################################
// # Functions for using AltAzPreferences custom class #
// #####################################################
// Load preferences from "eeprom" (flash memory on ESP32) into global variables
void LoadPreferencesIntoMemory();

#endif // FUNCTIONS_ALTAZPREFERENCES_H
