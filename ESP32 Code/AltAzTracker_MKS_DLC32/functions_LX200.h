#pragma once
#ifndef FUNCTIONS_LX200_H
#define FUNCTIONS_LX200_H

// #########################
// # LX200 Serial Commands #
// #########################

#include <Arduino.h>

//Receive data for LX200 telescope. Begins with :, ends with #
void serialRecieveLX200();

void serialClearBuffer();

void serialParseData();

#endif // FUNCTIONS_LX200_H
