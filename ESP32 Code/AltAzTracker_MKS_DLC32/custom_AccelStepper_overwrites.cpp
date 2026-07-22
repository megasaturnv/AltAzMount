#include "custom_AccelStepper_overwrites.h"
#include "functions_shift_register.h"
#include "global_variables.h"

// ##########################################
// # Custom AccelStepper Library Overwrites #
// ##########################################

// Constructor: pass the bit positions for step and dir
AccelStepper_ShiftRegister::AccelStepper_ShiftRegister ( uint8_t stepBit_,  uint8_t dirBit_)
  : AccelStepper(AccelStepper::FUNCTION, stepBit_, dirBit_), stepBit(stepBit_), dirBit(dirBit_) {}
/*
  //virtual void digitalWriteWrapper(uint8_t pin, uint8_t val) override {
  virtual void digitalWriteWrapper(uint8_t pin, uint8_t val) {
  uint8_t bitToSet = (pin == 0) ? stepBit : dirBit;
  if (val) srState |= (1 << bitToSet);
  else     srState &= ~(1 << bitToSet);

  srWriteBitbang(srState);
  }*/

// Override the low-level step pulse function from AccelStepper
void AccelStepper_ShiftRegister::step(long step)
{
  (void)step; // Unused

  // Map _direction to DIR bit
  if (_direction)
    srState |= (1 << dirBit);
  else
    srState &= ~(1 << dirBit);

  // Pulse STEP bit HIGH
  srState |= (1 << stepBit);
  srWriteBitbang(srState);

  // Respect minimum pulse width
  delayMicroseconds(minPulseWidth);

  // STEP LOW
  srState &= ~(1 << stepBit);
  srWriteBitbang(srState);
}

// Global instances
AccelStepper_ShiftRegister stepperAlt(SR_ALT_STEP_BIT, SR_ALT_DIR_BIT);
AccelStepper_ShiftRegister stepperAz(SR_AZ_STEP_BIT, SR_AZ_DIR_BIT);
