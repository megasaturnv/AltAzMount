#pragma once
#ifndef FUNCTIONS_CALCULATE_RA_DEC_OF_OBJECT_H
#define FUNCTIONS_CALCULATE_RA_DEC_OF_OBJECT_H

#include <ESP32Time.h>

#include "structures.h"

structRADec getRADecOfObject_Sun(ESP32Time &rtc);
structRADec getRADecOfObject_Moon(ESP32Time &rtc);

void setTargetToObject(char *strObject);

#endif // FUNCTIONS_CALCULATE_RA_DEC_OF_OBJECT_H
