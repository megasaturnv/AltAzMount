#pragma once
#ifndef FUNCTIONS_GLOBAL_VARIABLES_H
#define FUNCTIONS_GLOBAL_VARIABLES_H

#if DISPLAY_OUTPUT == SSD1306
  #include <SSD1306Ascii.h>
  #include <SSD1306AsciiWire.h>
#elif DISPLAY_OUTPUT == SH1106
  #include <U8g2lib.h>
#endif

// Date and Time
#include <ESP32Time.h>
#include <RTClib.h>

#include "structures.h"
#include "settings.h"
#include "class_AltAzPreferences.h"


// ####################
// # Global Variables #
// ####################
#if DISPLAY_OUTPUT == SSD1306
  extern SSD1306AsciiWire oled; //oled(wire1)
#elif DISPLAY_OUTPUT == SH1106
  //extern U8G2_SH1106_128X64_NONAME_F_HW_I2C oled;
  extern U8G2_SH1106_128X64_NONAME_F_HW_I2C oled;
  //extern U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2;
#endif

extern RTC_DS3231 rtcDS3231;
extern ESP32Time rtcESP32Time;

extern class_AltAzPreferences altAzPreferences;

// Shift register state byte
extern uint8_t srState;

//struct used when setting a new date and time
extern struct tm setNewDateAndTime;

//OLED print debug level
enum debugLevel { INFO, WARNING, ERROR };

extern debugLevel setting_OledPrintDebugLevel;

extern char wifiSsid[33]; //WIFI_SSID_MAX_LENGTH
extern char wifiPassword[65]; //WIFI_PASSWORD_MAX_LENGTH

extern double latitudeDecimal;
extern int latitudeDMSDegrees;
extern int latitudeDMSMinutes;
extern double latitudeDMSSeconds;
extern double longitudeDecimal;
extern int longitudeDMSDegrees;
extern int longitudeDMSMinutes;
extern double longitudeDMSSeconds;

//extern bool moveSteppersToTargetSteps;
extern bool moveSteppersToTargetStepsAlt;
extern bool moveSteppersToTargetStepsAz;
extern long stepperAltTargetSteps;
extern long stepperAzTargetSteps;

extern bool serialNewData;

extern bool LX200HighPrecisionMode;
extern structHoursMinutesSeconds LX200TargetRA;
extern structDegreesMinutesSeconds LX200TargetDec;
extern float LX200TargetAlt;
extern float LX200TargetAz;

// An array to store the received data on the serial port
//extern char serialReceivedChars[SERIAL_BUFFER_SIZE];
extern char serialReceivedChars[32];

//Store if the device has been homed for each axis. False means device shouldn't move unless it is homing
extern bool hasHomePositionAlt;
extern bool hasHomePositionAz;

extern double angleOffsetAlt;
extern double angleOffsetAz;

//Tripod mode. True = Alt/Az mode. False = RA/Dec Equatorial mode - tripod has been angled at user's latitude on north direction for northern hemisphere.
extern bool tripodModeAltAz;

extern unsigned long updateTrackingRADecPreviousMillis;
extern unsigned long updateTrackingRADecCurrentMillis;
extern bool trackingRADecTarget;

#endif // FUNCTIONS_GLOBAL_VARIABLES_H
