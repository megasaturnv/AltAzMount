#include "functions_Calculate_RA_Dec_Of_Object.h"

#include <Arduino.h>
#include <ESP32Time.h>
#include <math.h>

#include "structures.h"
#include "global_variables.h"
#include "functions_oled_print.h"
#include "functions_AltAz_RADec_conversion.h"

structRADec getRADecOfObject_Sun(ESP32Time &rtc) {
    // Standard low-precision solar position algorithm

    // Get UTC date/time from ESP32Time
    int year   = rtc.getYear();
    int month  = rtc.getMonth() + 1;   // ESP32Time months are typically 0-11
    int day    = rtc.getDay();
    int hour   = rtc.getHour(true);    // 24-hour format
    int minute = rtc.getMinute();
    int second = rtc.getSecond();

    double JD = julianDate(year, month, day, hour, minute, second);

    // Days since J2000.0
    double n = JD - 2451545.0;

    // Mean longitude of the Sun (deg)
    double L = fmod(280.460 + 0.9856474 * n, 360.0);
    if (L < 0) L += 360.0;

    // Mean anomaly (deg)
    double g = fmod(357.528 + 0.9856003 * n, 360.0);
    if (g < 0) g += 360.0;

    double gRad = g * DEG_TO_RAD;

    // Ecliptic longitude (deg)
    double lambda = L + 1.915 * sin(gRad) + 0.020 * sin(2.0 * gRad);

    // Obliquity of the ecliptic (deg)
    double epsilon = 23.439 - 0.0000004 * n;

    double lambdaRad = lambda * DEG_TO_RAD;
    double epsilonRad = epsilon * DEG_TO_RAD;

    // Right Ascension and Declination
    double alpha = atan2(cos(epsilonRad) * sin(lambdaRad),
                         cos(lambdaRad));

    double delta = asin(sin(epsilonRad) * sin(lambdaRad));

    // Convert RA to degrees in range 0-360
    double alphaDeg = alpha * RAD_TO_DEG;
    if (alphaDeg < 0) alphaDeg += 360.0;

    // Convert RA degrees to hours
    double raHoursTotal = alphaDeg / 15.0;

    int raHours = (int)raHoursTotal;
    double raMinutesTotal = (raHoursTotal - raHours) * 60.0;

    int raMinutes = (int)raMinutesTotal;
    double raSeconds = (raMinutesTotal - raMinutes) * 60.0;

    // Convert Dec to DMS
    double decDegTotal = delta * RAD_TO_DEG;

    int sign = (decDegTotal < 0) ? -1 : 1;
    double absDec = fabs(decDegTotal);

    int decDegrees = (int)absDec;
    double decMinutesTotal = (absDec - decDegrees) * 60.0;

    int decMinutes = (int)decMinutesTotal;
    double decSeconds = (decMinutesTotal - decMinutes) * 60.0;

    decDegrees *= sign;

    structRADec result;

    result.rightAscension.hours = raHours;
    result.rightAscension.minutes = raMinutes;
    result.rightAscension.seconds = raSeconds;

    result.declination.degrees = decDegrees;
    result.declination.minutes = decMinutes;
    result.declination.seconds = decSeconds;

    return result;
}

structRADec getRADecOfObject_Moon(ESP32Time &rtc) {
    //Uses a truncated version of the Meeus algorithm
    int year   = rtc.getYear();
    int month  = rtc.getMonth() + 1;
    int day    = rtc.getDay();
    int hour   = rtc.getHour(true);
    int minute = rtc.getMinute();
    int second = rtc.getSecond();

    double JD = julianDate(year, month, day, hour, minute, second);

    // Days since J2000.0
    double d = JD - 2451543.5;

    // Mean orbital elements (degrees)
    double N = fmod(125.1228 - 0.0529538083 * d, 360.0);
    double i = 5.1454;
    double w = fmod(318.0634 + 0.1643573223 * d, 360.0);
    double a = 60.2666;     // Earth radii
    double e = 0.054900;
    double M = fmod(115.3654 + 13.0649929509 * d, 360.0);

    if (N < 0) N += 360.0;
    if (w < 0) w += 360.0;
    if (M < 0) M += 360.0;

    double Mrad = M * DEG_TO_RAD;

    // Eccentric anomaly
    double E = M + RAD_TO_DEG * e * sin(Mrad) * (1.0 + e * cos(Mrad));
    double Erad = E * DEG_TO_RAD;

    // Moon position in orbital plane
    double xv = a * (cos(Erad) - e);
    double yv = a * (sqrt(1.0 - e * e) * sin(Erad));

    double v = atan2(yv, xv);
    double r = sqrt(xv * xv + yv * yv);

    double Nrad = N * DEG_TO_RAD;
    double wrad = w * DEG_TO_RAD;
    double irad = i * DEG_TO_RAD;

    // Ecliptic coordinates
    double xh = r * (cos(Nrad) * cos(v + wrad)
    - sin(Nrad) * sin(v + wrad) * cos(irad));

    double yh = r * (sin(Nrad) * cos(v + wrad)
    + cos(Nrad) * sin(v + wrad) * cos(irad));

    double zh = r * sin(v + wrad) * sin(irad);

    double lonecl = atan2(yh, xh);
    double latecl = atan2(zh, sqrt(xh * xh + yh * yh));

    // -------- Largest perturbation terms --------

    double Ms = fmod(356.0470 + 0.9856002585 * d, 360.0); // Sun mean anomaly
    double Ls = fmod(280.4600 + 0.98564736 * d, 360.0);   // Sun mean longitude

    double Lm = fmod(N + w + M, 360.0);                   // Moon mean longitude
    double D  = fmod(Lm - Ls, 360.0);                     // Mean elongation
    double F  = fmod(Lm - N, 360.0);                      // Argument of latitude

    lonecl += DEG_TO_RAD * (
        -1.274 * sin((M - 2.0 * D) * DEG_TO_RAD)
        +0.658 * sin((2.0 * D) * DEG_TO_RAD)
        -0.186 * sin(Ms * DEG_TO_RAD)
        -0.059 * sin((2.0 * M - 2.0 * D) * DEG_TO_RAD)
        -0.057 * sin((M - 2.0 * D + Ms) * DEG_TO_RAD)
        +0.053 * sin((M + 2.0 * D) * DEG_TO_RAD)
        +0.046 * sin((2.0 * D - Ms) * DEG_TO_RAD)
        +0.041 * sin((M - Ms) * DEG_TO_RAD)
    );

    latecl += DEG_TO_RAD * (
        -0.173 * sin((F - 2.0 * D) * DEG_TO_RAD)
        -0.055 * sin((M - F - 2.0 * D) * DEG_TO_RAD)
        -0.046 * sin((M + F - 2.0 * D) * DEG_TO_RAD)
        +0.033 * sin((F + 2.0 * D) * DEG_TO_RAD)
        +0.017 * sin((2.0 * M + F) * DEG_TO_RAD)
    );

    // Obliquity
    double eps = (23.4393 - 3.563E-7 * d) * DEG_TO_RAD;

    // Equatorial coordinates
    double xe = cos(lonecl) * cos(latecl);
    double ye = sin(lonecl) * cos(latecl) * cos(eps)
    - sin(latecl) * sin(eps);
    double ze = sin(lonecl) * cos(latecl) * sin(eps)
    + sin(latecl) * cos(eps);

    double RA = atan2(ye, xe);
    double Dec = atan2(ze, sqrt(xe * xe + ye * ye));

    // Convert RA to 0-24 hours
    double RAdeg = RA * RAD_TO_DEG;
    if (RAdeg < 0) RAdeg += 360.0;

    double totalHours = RAdeg / 15.0;

    int raHours = (int)totalHours;
    double totalMinutes = (totalHours - raHours) * 60.0;

    int raMinutes = (int)totalMinutes;
    double raSeconds = (totalMinutes - raMinutes) * 60.0;

    // Convert Dec to DMS
    double decDeg = Dec * RAD_TO_DEG;

    int sign = (decDeg < 0) ? -1 : 1;
    double absDec = fabs(decDeg);

    int decDegrees = (int)absDec;
    double decMinutesTotal = (absDec - decDegrees) * 60.0;

    int decMinutes = (int)decMinutesTotal;
    double decSeconds = (decMinutesTotal - decMinutes) * 60.0;

    decDegrees *= sign;

    structRADec result;

    result.rightAscension.hours = raHours;
    result.rightAscension.minutes = raMinutes;
    result.rightAscension.seconds = raSeconds;

    result.declination.degrees = decDegrees;
    result.declination.minutes = decMinutes;
    result.declination.seconds = decSeconds;

    return result;
}

void setTargetToObject(char *strObject) {
  OLED_print("Target to object", INFO);
  
  structRADec objectRADec;
  bool validObject = false;
  if (strObject == "Sun") {
    objectRADec = getRADecOfObject_Moon(rtc);
    validObject = true;
  } else if (strObject == "Moon") {
    objectRADec = getRADecOfObject_Sun(rtc);
    validObject = true;
  } else {
    OLED_print("Target to unknown object", WARNING);
  }

  if (validObject) {
    LX200TargetRA = objectRADec.rightAscension;
    LX200TargetDec =  objectRADec.declination;

    Serial.println(LX200TargetRA.hours);
    Serial.println(LX200TargetDec.degrees);
  }
}



//Convert J2000 to current date. Used for non-solar system objects from cataglogue
//Example
//structRADec polarisJ2000;
//double JD = julianDate(rtc.getYear(), rtc.getMonth() + 1, rtc.getDay(), rtc.getHour(true), rtc.getMinute(), rtc.getSecond());
//structRADec polarisNow = precessJ2000ToDate(polarisJ2000, JD);

structRADec precessJ2000ToDate(
    structRADec j2000,
    double JD)
{
    double ra =
        15.0 *
        (j2000.rightAscension.hours +
         j2000.rightAscension.minutes / 60.0 +
         j2000.rightAscension.seconds / 3600.0);

    double dec =
        fabs(j2000.declination.degrees) +
        j2000.declination.minutes / 60.0 +
        j2000.declination.seconds / 3600.0;

    if (j2000.declination.degrees < 0)
        dec = -dec;

    ra *= DEG_TO_RAD;
    dec *= DEG_TO_RAD;

    double T = (JD - 2451545.0) / 36525.0;

    double zeta =
        (2306.2181 * T +
         0.30188 * T * T +
         0.017998 * T * T * T) / 3600.0;

    double z =
        (2306.2181 * T +
         1.09468 * T * T +
         0.018203 * T * T * T) / 3600.0;

    double theta =
        (2004.3109 * T -
         0.42665 * T * T -
         0.041833 * T * T * T) / 3600.0;

    zeta *= DEG_TO_RAD;
    z *= DEG_TO_RAD;
    theta *= DEG_TO_RAD;

    double A =
        cos(dec) * sin(ra + zeta);

    double B =
        cos(theta) * cos(dec) * cos(ra + zeta)
        - sin(theta) * sin(dec);

    double C =
        sin(theta) * cos(dec) * cos(ra + zeta)
        + cos(theta) * sin(dec);

    double raNew =
        atan2(A, B) + z;

    double decNew =
        asin(C);

    if (raNew < 0)
        raNew += 2.0 * M_PI;

    double raHours =
        raNew * RAD_TO_DEG / 15.0;

    int h = (int)raHours;

    double raMinTotal =
        (raHours - h) * 60.0;

    int m = (int)raMinTotal;

    double s =
        (raMinTotal - m) * 60.0;

    double decDeg =
        decNew * RAD_TO_DEG;

    int sign = (decDeg < 0) ? -1 : 1;

    double absDec = fabs(decDeg);

    int d = (int)absDec;

    double decMinTotal =
        (absDec - d) * 60.0;

    int dm = (int)decMinTotal;

    double ds =
        (decMinTotal - dm) * 60.0;

    d *= sign;

    structRADec result;

    result.rightAscension.hours = h;
    result.rightAscension.minutes = m;
    result.rightAscension.seconds = s;

    result.declination.degrees = d;
    result.declination.minutes = dm;
    result.declination.seconds = ds;

    return result;
}
