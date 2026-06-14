#pragma once
#ifndef CUSTOM_ACCELSTEPPER_OVERWRITES_H
#define CUSTOM_ACCELSTEPPER_OVERWRITES_H

#include <AccelStepper.h>
#include <Arduino.h>

// ##########################################
// # Custom AccelStepper Library Overwrites #
// ##########################################

// Custom AccelStepper interface
class AccelStepper_ShiftRegister : public AccelStepper {
public:
  unsigned int minPulseWidth = 1; // We cannot use _minPulseWidth as it is private

  // Constructor: pass the bit positions for step and dir
  AccelStepper_ShiftRegister(uint8_t stepBit_, uint8_t dirBit_);

protected:
  uint8_t stepBit;
  uint8_t dirBit;

  // Override the low-level step pulse function from AccelStepper
  void step(long step) override;

  // Override the low-level step pulse function from AccelStepper
  void step1(long step) override;
};

// Optional: if instances should be globally available
extern AccelStepper_ShiftRegister stepperAlt;
extern AccelStepper_ShiftRegister stepperAz;

#endif // CUSTOM_ACCELSTEPPER_OVERWRITES_H
