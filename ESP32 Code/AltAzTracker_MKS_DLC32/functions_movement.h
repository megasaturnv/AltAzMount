#pragma once
#ifndef FUNCTIONS_MOVEMENT_H
#define FUNCTIONS_MOVEMENT_H

#include <AccelStepper.h>
#include "structures.h"


// ##########################
// # Functions for movement #
// ##########################
// Stepper Motors
void steppersEnable(bool argEnable);

//Disable stepper motors and stop all further code execution
void steppersDisableAndAbortAllCode();

//Sets the current position of this stepper motor to be angle supplied. Does not move the stepper.
bool stepperMotorSetCurrentPositionToAngle(AccelStepper &stepper, float argAngle, bool moveInPositiveDirection);

//Sets the current position of the az motor to be angle supplied. Does not move the stepper. Sets hasHomePositionAz to true if setting was successful
void azStepperMotorSetCurrentPositionToAngle(float argAngle);

//Home the stepper motor given
bool stepperMotorHome(AccelStepper &stepper, uint8_t homeLimitSwitchPin, uint8_t endLimitSwitchPin, bool moveInPositiveDirection);

// Home the Altitude axis
bool performAltHome();

// Home the Azimuth axis
bool performAzHome();

// Perform home on each axis if needed
void performHomeOnlyIfNeededAndSetHasHome();

long convertAngleToTargetSteps(float targetAngle, bool moveInPositiveDirection);

// Moves stepper motor given to an angle. Returns true if successful, false if not
bool stepperMotorMoveToAngleFromHome(AccelStepper &stepper, uint8_t homeLimitSwitchPin, uint8_t endLimitSwitchPin, float minimumAngle, float maximumAngle, float targetAngle, bool moveInPositiveDirection);

//Altered for float and double values. Originally from arduino source code
//Args are: value, fromLow, fromHigh, toLow, toHigh
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
 //Args are: value, fromLow, fromHigh, toLow, toHigh
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

//Get position of the stepper from home. Should always be positive
double positionGetAngleFromHome(AccelStepper &stepper);

//Get angle of altitude. Is allowed to be negative, means currently pointing below horizon
double positionGetAngleAlt();

//Get angle of azimuth
double positionGetAngleAz();

//Get HMS Right Ascension of current position
structHoursMinutesSeconds positionGetHMSRA();

//Get DMS Declination of current position
structDegreesMinutesSeconds positionGetDMSDec();

bool positionSetTargetAngleAltitude(float altAngle);
bool positionSetTargetAngleAzimuth(float azAngle);
bool positionSetTargetAltAz(structAltAz altAzAngle);

bool positionSetTargetAltAz(float altAngle, float azAngle);

//Stop stepper motors in their current position
bool steppersHaltMovement();

bool positionSetTargetRADec(structHoursMinutesSeconds ra, structDegreesMinutesSeconds dec);

#endif // FUNCTIONS_MOVEMENT_H
