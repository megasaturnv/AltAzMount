#pragma once
#ifndef FUNCTIONS_STRUCTURES_H
#define FUNCTIONS_STRUCTURES_H


// ##############
// # Structures #
// ##############
struct structLatLong {
  double latitudeDeg;
  double longitudeDeg;
};

struct structAltAz {
  float altitudeDeg;
  float azimuthDeg;
};

struct structHoursMinutesSeconds {
  int hours;
  int minutes;
  double seconds;
};

struct structDegreesMinutesSeconds {
  int degrees;
  int minutes;
  double seconds;
};

struct structRADec {
  structHoursMinutesSeconds rightAscension;
  structDegreesMinutesSeconds declination;
};

#endif // FUNCTIONS_STRUCTURES_H
