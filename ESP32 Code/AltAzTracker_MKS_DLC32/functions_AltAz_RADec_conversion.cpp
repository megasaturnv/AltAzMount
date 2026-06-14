#include "functions_AltAz_RADec_conversion.h"
#include <Arduino.h>
#include <math.h>

// ######################################
// # Alt/Az RA/Dec conversion functions #
// ######################################

// Various notes regarding calculation of RA and Dec
// https://projecthub.arduino.cc/paulplusx/calculation-of-right-ascension-and-declination-e00496
// https://github.com/Bill-Gray/lunar/blob/master/alt_az.cpp
// https://jonvoisey.net/blog/2018/07/data-converting-alt-az-to-ra-dec-derivation/

//Maths constants for converting to and from radians
//Todo, look into using DEG_TO_RAD and RAD_TO_DEG built-in variables. Look into if they are double or float and compare precision
const double DEG_TO_RAD_FACTOR = (M_PI / 180.0);
const double RAD_TO_DEG_FACTOR = (180.0 / M_PI);

// Degrees and hours utility functions
double normalizeDegrees(double deg) {
  while (deg < 0.0) deg += 360.0;
  while (deg >= 360.0) deg -= 360.0;
  return deg;
}

double normalizeHours(double hours) {
  while (hours < 0.0) hours += 24.0;
  while (hours >= 24.0) hours -= 24.0;
  return hours;
}

// structHoursMinutesSeconds to decimal degrees
double hmsToDegrees(structHoursMinutesSeconds ra) {
  return (ra.hours + ra.minutes / 60.0 + ra.seconds / 3600.0) * 15.0;
}

// structDegreesMinutesSeconds to decimal degrees
double dmsToDegrees(structDegreesMinutesSeconds dms) {
  double sign = (dms.degrees < 0) ? -1.0 : 1.0;

  double absDeg = fabs(dms.degrees)
  + dms.minutes / 60.0
  + dms.seconds / 3600.0;

  return sign * absDeg;
}

// Degrees Minutes Seconds in separate args to decimal degrees
double dmsToDegrees(float deg, float min, float sec) {
  double sign = (deg < 0) ? -1.0 : 1.0;

  double absDeg = fabs(deg)
  + min / 60.0
  + sec / 3600.0;

  return sign * absDeg;
}

// Convert to Julian Date
double julianDate(int year, int month, int day, int hour, int minute, int second) {
  if (month <= 2) {
    year -= 1;
    month += 12;
  }

  int centuaryNumber = year / 100;
  int gregorianCorrection = 2 - centuaryNumber + (centuaryNumber / 4);

  double dayFraction = (hour + minute / 60.0 + second / 3600.0) / 24.0;

  double JD = floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + gregorianCorrection  - 1524.5  + dayFraction;

  return JD;
}

// Greenwich Mean Sidereal Time
double gmstDegrees(double jd) {
  double T = (jd - 2451545.0) / 36525.0;
  double gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) + 0.000387933 * T * T - (T * T * T) / 38710000.0;
  return normalizeDegrees(gmst);
}

// Convert decimal hours to structHoursMinutesSeconds
structHoursMinutesSeconds decimalHoursToHMS(double hours) {
  structHoursMinutesSeconds out;

  hours = normalizeHours(hours);

  out.hours = (int)hours;

  double rem = (hours - out.hours) * 60.0;
  out.minutes = (int)rem;

  out.seconds = (rem - out.minutes) * 60.0;

  return out;
}

// Convert decimal degrees to structDegreesMinutesSeconds
structDegreesMinutesSeconds decimalDegreesToDMS(double degrees) {
  structDegreesMinutesSeconds out;

  int sign = (degrees < 0) ? -1 : 1;

  degrees = fabs(degrees);

  out.degrees = (int)degrees * sign;

  double rem = (degrees - (int)degrees) * 60.0;
  out.minutes = (int)rem;

  out.seconds = (rem - out.minutes) * 60.0;

  return out;
}

// Alt/Az to RA/Dec (JNow) Main Conversion Function
structRADec altAzToRaDecJNow(double altitudeDeg, double azimuthDeg, double latitudeDeg, double longitudeDeg, int year, int month, int day, int hour, int minute, int second) {
  // Convert to radians
  double alt = altitudeDeg * DEG_TO_RAD_FACTOR;
  double az  = azimuthDeg * DEG_TO_RAD_FACTOR;
  double lat = latitudeDeg * DEG_TO_RAD_FACTOR;

  // Julian Date
  double jd = julianDate(year, month, day, hour, minute, second);

  // Local Sidereal Time
  double gmst = gmstDegrees(jd);

  // Longitude - East is positive
  double lstDeg = normalizeDegrees(gmst + longitudeDeg);

  // Declination
  double sinDec = sin(alt) * sin(lat) + cos(alt) * cos(lat) * cos(az);
  double dec = asin(sinDec);

  // Hour Angle
  double y = -sin(az) * cos(alt);
  double x = sin(alt) * cos(lat) - cos(alt) * sin(lat) * cos(az);
  double ha = atan2(y, x);
  double haDeg = normalizeDegrees(ha * RAD_TO_DEG_FACTOR);

  // Right Ascension
  double raDeg = normalizeDegrees(lstDeg - haDeg);

  // Convert RA degrees to hours
  double raHours = raDeg / 15.0;

  // Prepare output
  structRADec RADecReturnVal;

  RADecReturnVal.rightAscension = decimalHoursToHMS(raHours);
  RADecReturnVal.declination = decimalDegreesToDMS(dec * RAD_TO_DEG_FACTOR);

  return RADecReturnVal;
}

// RA/Dec (JNow) to Alt/Az Main Conversion Function
structAltAz raDecToAltAzJNow(structHoursMinutesSeconds raHMS, structDegreesMinutesSeconds decDMS, double latitudeDeg, double longitudeDeg, int year, int month, int day, int hour, int minute, int second) {
  // Convert inputs to decimal degrees
  double raDeg = hmsToDegrees(raHMS);
  double decDeg = dmsToDegrees(decDMS);

  // Convert input decimal degrees to radians
  double ra = raDeg * DEG_TO_RAD_FACTOR;
  double dec = decDeg * DEG_TO_RAD_FACTOR;
  double lat = latitudeDeg * DEG_TO_RAD_FACTOR;

  // Get Julian datetime from current datetime
  double jd = julianDate(year, month, day, hour, minute, second);

  // Get Greenwich mean sidereal time
  double gmst = gmstDegrees(jd);
  // Get local solar time, in degrees
  double lst = (gmst + longitudeDeg);
  // Get local solar time, in radians
  lst = normalizeDegrees(lst) * DEG_TO_RAD_FACTOR;
  // Get hour angle
  double H = lst - ra;

  // Altitude
  double sinAlt = sin(dec) * sin(lat) + cos(dec) * cos(lat) * cos(H);
  double alt = asin(sinAlt);

  // Azimuth
  double y = -sin(H);
  double x = tan(dec) * cos(lat) - sin(lat) * cos(H);

  double az = atan2(y, x);

  // Convert outputs
  structAltAz AltAzReturnVal;
  AltAzReturnVal.altitudeDeg = alt * RAD_TO_DEG_FACTOR;
  AltAzReturnVal.azimuthDeg = normalizeDegrees(az * RAD_TO_DEG_FACTOR);

  return AltAzReturnVal;
}
