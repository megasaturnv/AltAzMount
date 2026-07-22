#include "settings.h"

// ##################
// # Const settings #
// ##################
// Defines are fine if the value is only referenced once. Else, it is more efficient to use const. For simplicity for now, using const whenever possible
const int SERIAL_BAUD_RATE   = 9600; //Standard baud rate for LX200 protocol is 9600, 8N1 configuration
const int SERIAL_BUFFER_SIZE = 32;

const int WIFI_SSID_MAX_LENGTH     = 33;
const int WIFI_PASSWORD_MAX_LENGTH = 65;

const char hostname[] = "MKS-DLC32";
const unsigned long WIFI_TIMEOUT_MS = 10000; // 10 seconds

const char defaultPreference_wifiSsid[] = "WiFiSSID";
const char defaultPreference_wifiPassword[] = "WiFiPassword";

const double defaultPreference_latitude  = 51.447;
const double defaultPreference_longitude = 0.0;

const uint8_t serialNumChars                    = 32;
const unsigned long updateTrackingRADecInterval = 1000; //When we are tracking an object via RA Dec coordinates, how often to re-compute position

// Stepper motor settings
const float STEPPER_STEPS_PER_REV                             = 144000.0; // 200 (steps) * 16 (1/16 microstepping) * 90 (gear teeth) / 2 (2-start worm gear) = 360 degrees (0.0025° per step)
const int WITHIN_STEPS_TO_ARRIVE_AT_TARGET                    = 16;
const float ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH          = 2.5; //Move 2 degrees away from limit switch if it is depressed while trying to home
const float ANGLE_ALLOWED_WITHIN_LIMIT_SWITCH_BEING_DEPRESSED = 2.5; //Within this angle of the limit switch, ignore if the limit switch is pressed as this is expected
const bool STEPPER_ALT_MOVE_IN_POSITIVE_DIRECTION             = false;
const bool STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION              = true;

const float X_STEPPER_MINIMUM_ANGLE              = 0.0; //Home posiiton should always be set at 0. No need to change this
const float X_STEPPER_MAXIMUM_ANGLE              = 360.0; //todo
const float X_STEPPER_ANGLE_OF_NORTH_FROM_HOME   = 0.0;  //todo
const float X_STEPPER_ANGLE_OF_SOUTH_FROM_HOME   = 180.0; //todo

const float Z_STEPPER_MINIMUM_ANGLE              = 0.0; //Home posiiton should always be set at 0. No need to change this
const float Z_STEPPER_MAXIMUM_ANGLE              = 130.0; //todo
const float Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME = 35.0; //todo
const float Z_STEPPER_ANGLE_OF_ZENITH_FROM_HOME  = 125.0; //todo

// I2S / 74HC595 stepper shift register
// Bit positions in the 595 output byte (I2SO.x in FluidNC terms)
const int SR_STEPPER_DISABLE_BIT = 0; // I2SO.0  - shared EN (active high disables)
const int SR_X_STEP_BIT          = 1; // I2SO.1
const int SR_X_DIR_BIT           = 2; // I2SO.2
const int SR_Z_STEP_BIT          = 3; // I2SO.3
const int SR_Z_DIR_BIT           = 4; // I2SO.4
const int SR_Y_STEP_BIT          = 5; // I2SO.5
const int SR_Y_DIR_BIT           = 6; // I2SO.6
const int SR_BEEPER_BIT          = 7; // I2SO.7. EXP1 LCD beeper and beeper Maybe EXP1 LCD beeper?

// Shift register
const int PIN_I2S_BCK  = 16; //Clock
const int PIN_I2S_WS   = 17; //Latch. Word select or left/right if I2S
const int PIN_I2S_DATA = 21; //Data

// Endstops (active-low to GND when triggered)
const int PIN_X_LIMIT = 36; // input-only
const int PIN_Y_LIMIT = 35; // input-only
const int PIN_Z_LIMIT = 34; // input-only
const int PIN_Z_PROBE = 22; // Z endstop / probe header. Note - due to wiring on the MKS DLC32 (Component D16 on schematic), never put an input signal on this line greater than 3.3v. Note the header is it found on contains 5v on the vcc pin

// Laser / spindle
//const int PIN_LASER_PWM  = 32; // TTL signal to laser module (0-5V PWM)
//const int LASER_PWM_CH   = 0;
//const int LASER_PWM_FREQ = 5000;
//const int LASER_PWM_RES  = 8;  // 8-bit -> 0..255

// SD card (SPI)
const int PIN_SD_CS     = 15;
const int PIN_SD_DETECT = 39; // input-only
const int PIN_SPI_SCK   = 14;
const int PIN_SPI_MOSI  = 13;
const int PIN_SPI_MISO  = 12; //2?

// EXP1 and EXP2 (TFT touchscreen - TS24/TS35)
//const int PIN_TOUCH_CS    = 0;  // touch controller CS (Maybe CS is pin 5?)
const int PIN_EXP1_5        = 5;  // EXP1 LCD_EN_0. Pin 5 cannot be pulled high or low on boot by an external circuit
const int PIN_EXP1_25_HC135 = 25; // EXP1 LCD_CS_0. Via HC135 buffer so output only
const int PIN_EXP1_26_HC135 = 26; //EXP1 LCD_TOUCH_CS_0. Via HC135 buffer so output only
const int PIN_EXP1_27_HC135 = 27; //EXP1 LCD_RST_0. Via HC135 buffer so output only
const int PIN_EXP1_BACK     = 33; // EXP1 LCD_RS
const int PIN_EXP2_CONFIRM  = 18; // EXP2 LCD_SCK
const int PIN_EXP2_TRA      = 23; // EXP2 LCD_MOSI. TRA and TRB seem to be backwards on OLED rotary encoder module? Or, PCINT is funky on an ESP32
const int PIN_EXP2_TRB      = 19; // EXP2 LCD_MISO. TRA and TRB seem to be backwards on OLED rotary encoder module? Or, PCINT is funky on an ESP32

//TTL pin
const int PIN_TTL = 32; //Also marked as LC in schematic

//I2C pins
const int OLED_SDA = 0;
const int OLED_SCL = 4;

//LED Pin
const int LED_PIN = 2;



//Pins assigned to Motors and limit switches
const int SR_ALT_STEP_BIT    = SR_Z_STEP_BIT;
const int SR_ALT_DIR_BIT     = SR_Z_DIR_BIT;
const int SR_AZ_STEP_BIT     = SR_X_STEP_BIT;
const int SR_AZ_DIR_BIT      = SR_X_DIR_BIT;
const int PIN_ALT_LIMIT_HOME = PIN_X_LIMIT;
const int PIN_ALT_LIMIT_END  = PIN_Y_LIMIT;
const int PIN_AZ_LIMIT_HOME  = PIN_Z_LIMIT;
const int PIN_AZ_LIMIT_END   = PIN_Z_PROBE;
