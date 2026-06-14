#include "functions_shift_register.h"
#include <Arduino.h>
#include "settings.h"
#include "global_variables.h"

// ################################
// # Functions for Shift Register #
// ################################
// Todo, look into application of static, inline for optimisation

// Bit-bang for the 595
// Todo, drive via the I2S peripheral DMA stream so step pulses are less cpu-intensive
void srWriteBitbang(uint8_t value) {
  digitalWrite(PIN_I2S_WS, LOW);
  shiftOut(PIN_I2S_DATA, PIN_I2S_BCK, MSBFIRST, value);
  digitalWrite(PIN_I2S_WS, HIGH);
  //Possibly use GPIO.out_w1ts/GPIO.out_w1tc register writes in the future for 10x faster bit banging?

  // old code
  /*
  digitalWrite(PIN_I2S_WS, LOW);
  for (int i = 7; i >= 0; --i) {
      digitalWrite(PIN_I2S_BCK, LOW);
      digitalWrite(PIN_I2S_DATA, (value >> i) & 1);
      digitalWrite(PIN_I2S_BCK, HIGH);
  }
  digitalWrite(PIN_I2S_WS, HIGH); // latch
  */
}

void srSetBit(uint8_t argBit, bool argValue) {
  if (argValue) srState |=  (1 << argBit);
  else    srState &= ~(1 << argBit);
  srWriteBitbang(srState);
}
