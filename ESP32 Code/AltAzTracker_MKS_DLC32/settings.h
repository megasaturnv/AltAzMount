#pragma once
#ifndef FUNCTIONS_SETTINGS_H
#define FUNCTIONS_SETTINGS_H

#include <Arduino.h>


// ###################
// # Define Settings #
// ###################
//todo, constexpr better?

// Defines are fine if the value is only referenced once. Else, it is more efficient to use const
extern const int SERIAL_BAUD_RATE;
extern const int SERIAL_BUFFER_SIZE;

// Display type IDs. No need to modify these, only used for logic
#define SSD1306 1
#define SH1106  2
// Select the display output. Supported screens: SSD1306, SH1106
#define DISPLAY_OUTPUT SSD1306


// ##################
// # Const settings #
// ##################
extern const double defaultPreference_latitude;
extern const double defaultPreference_longitude;

extern const uint8_t serialNumChars;
extern const unsigned long updateTrackingRADecInterval;

// Stepper motor settings
extern const float STEPPER_STEPS_PER_REV;
extern const int WITHIN_STEPS_TO_ARRIVE_AT_TARGET;
extern const float ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH;
extern const float ANGLE_ALLOWED_WITHIN_LIMIT_SWITCH_BEING_DEPRESSED;
extern const bool STEPPER_ALT_MOVE_IN_POSITIVE_DIRECTION;
extern const bool STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION;

extern const float X_STEPPER_MINIMUM_ANGLE;
extern const float X_STEPPER_MAXIMUM_ANGLE;
extern const float X_STEPPER_ANGLE_OF_NORTH_FROM_HOME;
extern const float X_STEPPER_ANGLE_OF_SOUTH_FROM_HOME;

extern const float Z_STEPPER_MINIMUM_ANGLE;
extern const float Z_STEPPER_MAXIMUM_ANGLE;
extern const float Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME;
extern const float Z_STEPPER_ANGLE_OF_ZENITH_FROM_HOME;

// I2S / 74HC595 stepper shift register
extern const int SR_STEPPER_DISABLE_BIT;
extern const int SR_X_STEP_BIT;
extern const int SR_X_DIR_BIT;
extern const int SR_Z_STEP_BIT;
extern const int SR_Z_DIR_BIT;
extern const int SR_Y_STEP_BIT;
extern const int SR_Y_DIR_BIT;
extern const int SR_BEEPER_BIT;

// Shift register
extern const int PIN_I2S_BCK;
extern const int PIN_I2S_WS;
extern const int PIN_I2S_DATA;

// Endstops (active-low to GND when triggered)
extern const int PIN_X_LIMIT;
extern const int PIN_Y_LIMIT;
extern const int PIN_Z_LIMIT;
extern const int PIN_Z_PROBE;

// Laser / spindle
//extern const int PIN_LASER_PWM;
//extern const int LASER_PWM_CH;
//extern const int LASER_PWM_FREQ;
//extern const int LASER_PWM_RES;

// SD card (SPI)
extern const int PIN_SD_CS;
extern const int PIN_SD_DETECT;
extern const int PIN_SPI_SCK;
extern const int PIN_SPI_MOSI;
extern const int PIN_SPI_MISO;

// EXP1 and EXP2 (TFT touchscreen - TS24/TS35)
//extern const int PIN_TOUCH_CS;
extern const int PIN_EXP1_5;
extern const int PIN_EXP1_25_HC135;
extern const int PIN_EXP1_26_HC135;
extern const int PIN_EXP1_27_HC135;
extern const int PIN_EXP1_BACK;
extern const int PIN_EXP2_CONFIRM;
extern const int PIN_EXP2_TRA;
extern const int PIN_EXP2_TRB;

//TTL pin
extern const int PIN_TTL;

//I2C pins
extern const int OLED_SDA;
extern const int OLED_SCL;

//LED Pin
extern const int LED_PIN;

#endif // FUNCTIONS_SETTINGS_H
