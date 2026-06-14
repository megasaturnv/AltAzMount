#pragma once
#ifndef FUNCTIONS_ARDUINO_MENU_H
#define FUNCTIONS_ARDUINO_MENU_H

#include <Arduino.h>

// ################
// # Menu Defines #
// ################
/*config.h*/

/* List of Supported Fonts
 *
 *  Arial14,
 *  Arial_bold_14,
 *  Callibri11,
 *  Callibri11_bold,
 *  Callibri11_italic,
 *  Callibri15,
 *  Corsiva_12,
 *  fixed_bold10x15,
 *  font5x7,    //Do not use in LARGE_FONT, can use as default font
 *  font8x8,
 *  Iain5x7,    //Do not use in LARGE_FONT, can use as default font
 *  lcd5x7,     //Do not use in LARGE_FONT, can use as default font
 *  Stang5x7,   //Do not use in LARGE_FONT, can use as default font
 *  System5x7,  //Do not use in LARGE_FONT, can use as default font
 *  TimesNewRoman16,
 *  TimesNewRoman16_bold,
 *  TimesNewRoman16_italic,
 *  utf8font10x16,
 *  Verdana12,
 *  Verdana12_bold,
 *  Verdana12_italic,
 *  X11fixed7x14,
 *  X11fixed7x14B,
 *  ZevvPeep8x16
 *
 */

#define OLED_I2C_ADDRESS 0x3c //3C     //Defined OLED I2C Address

/*
 * Define your font from the list.
 * Default font: lcd5x7
 * Comment out the following for using the default font.
 */
//#define LARGE_FONT Verdana12

#define TOTAL_NAV_BUTTONS 2       // Total Navigation Button used

//#define ENCODER_SENSITIVITY 8
#define ENCODER_SENSITIVITY 16

/*Demonstrate PWM with LED on D11*/
#define LED_PIN 2                //Defined LED Pin to D11

#define MAX_DEPTH 3

#ifdef LOC
// #define LARGE_FONT
#define INV
#endif

#endif // FUNCTIONS_ARDUINO_MENU_H
