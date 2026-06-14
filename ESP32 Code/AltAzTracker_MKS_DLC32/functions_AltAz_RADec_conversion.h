#pragma once
#ifndef FUNCTIONS_ALTAZ_RADEC_CONVERSION_H
#define FUNCTIONS_ALTAZ_RADEC_CONVERSION_H

#include <math.h>

#include "structures.h"

// ######################################
// # Alt/Az RA/Dec conversion functions #
// ######################################

//Maths constants for converting to and from radians
extern const double DEG_TO_RAD_FACTOR;
extern const double RAD_TO_DEG_FACTOR;

// Function declarations
double normalizeDegrees(double deg);
double normalizeHours(double hours);
double hmsToDegrees(structHoursMinutesSeconds ra);
double dmsToDegrees(structDegreesMinutesSeconds dec);
double dmsToDegrees(float deg, float min, float sec);
double julianDate(int year, int month, int day, int hour, int minute, int second);
double gmstDegrees(double jd);
structHoursMinutesSeconds decimalHoursToHMS(double hours);
structDegreesMinutesSeconds decimalDegreesToDMS(double degrees);
structRADec altAzToRaDecJNow(double altitudeDeg, double azimuthDeg, double latitudeDeg, double longitudeDeg, int year, int month, int day, int hour, int minute, int second);
structAltAz raDecToAltAzJNow(structHoursMinutesSeconds raHMS, structDegreesMinutesSeconds decDMS, double latitudeDeg, double longitudeDeg, int year, int month, int day, int hour, int minute, int second);

#endif // FUNCTIONS_ALTAZ_RADEC_CONVERSION_H
