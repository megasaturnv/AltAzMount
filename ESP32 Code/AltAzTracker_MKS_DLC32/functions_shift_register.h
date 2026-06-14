#pragma once
#ifndef FUNCTIONS_SHIFT_REGISTER_H
#define FUNCTIONS_SHIFT_REGISTER_H

// ################################
// # Functions for Shift Register #
// ################################
#include <Arduino.h>

// Function declarations
void srWriteBitbang(uint8_t value);
void srSetBit(uint8_t argBit, bool argValue);

#endif // FUNCTIONS_SHIFT_REGISTER_H
