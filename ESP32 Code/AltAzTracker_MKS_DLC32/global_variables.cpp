#include "global_variables.h"

#include "settings.h"


// ####################
// # Global Variables #
// ####################
#if DISPLAY_OUTPUT == SSD1306
  SSD1306AsciiWire oled; //oled(wire1)
#elif DISPLAY_OUTPUT == SH1106
  //U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0);
  U8G2_SH1106_128X64_NONAME_F_HW_I2C oled(U8G2_R0, U8X8_PIN_NONE, 0, 4);
  //U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 0, 4);
#endif

RTC_DS3231 rtcDS3231;
ESP32Time rtcESP32Time;

class_AltAzPreferences altAzPreferences;

// Shift register state byte
uint8_t srState = 0;
//static volatile uint8_t srState = 0;

//struct used when setting a new date and time
struct tm setNewDateAndTime;

// OLED print debug level
debugLevel setting_OledPrintDebugLevel = WARNING; //INFO will print INFO, WARNING AND ERROR. WARN will print WARNING, ERROR. ERROR will print only ERRORs

double latitudeDecimal = 0;
int latitudeDMSDegrees = 0;
int latitudeDMSMinutes = 0;
double latitudeDMSSeconds = 0;
double longitudeDecimal = 0;
int longitudeDMSDegrees = 0;
int longitudeDMSMinutes = 0;
double longitudeDMSSeconds = 0;

//bool moveSteppersToTargetSteps = false;
bool moveSteppersToTargetStepsAlt = false;
bool moveSteppersToTargetStepsAz = false;
long stepperAltTargetSteps;
long stepperAzTargetSteps;

bool serialNewData = false;

bool LX200HighPrecisionMode = false; // High or low precision mode for LX200 protocol. Low precision mode by default
structHoursMinutesSeconds LX200TargetRA; // Tacker moves here after :MS#
structDegreesMinutesSeconds LX200TargetDec; // Tracker moves here after :MS#
float LX200TargetAlt; // Tracker moves here after :MA#
float LX200TargetAz; // Tracker moves here after :MA#

// An array to store the received data on the serial port
//char serialReceivedChars[SERIAL_BUFFER_SIZE]; //todo, investigate "size of array 'serialReceivedChars' is not an integral constant-expression"
char serialReceivedChars[32];

// Store if the device has been homed for each axis. False means device shouldn't move unless it is homing'
bool hasHomePositionAlt = false;
bool hasHomePositionAz = false;

double angleOffsetAlt = 0.0; // Live adjustment of altitude. Currently -45 to 45 range allowed
double angleOffsetAz = 0.0; // Live adjustment of azimuth. Currently -45 to 45 range allowed

// Tripod mode. True = Alt/Az mode. False = RA/Dec Equatorial mode - tripod has been angled at user's latitude on north direction for northern hemisphere.
bool tripodModeAltAz = true;

unsigned long updateTrackingRADecPreviousMillis = 0;
unsigned long updateTrackingRADecCurrentMillis = 0;
bool trackingRADecTarget = false;
