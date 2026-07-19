// ##########################
// # Functions for movement #
// ##########################
#include <AccelStepper.h>

#include "functions_movement.h"
#include "structures.h"
#include "settings.h"
#include "functions_shift_register.h"
#include "global_variables.h"
#include "custom_AccelStepper_overwrites.h"
#include "functions_oled_print.h"
#include "functions_AltAz_RADec_conversion.h"

// Note that double has reduced performance over float on an esp32 https://www.reddit.com/r/esp32/comments/1la8fob/esp32_floating_point_performance/

// Stepper Motors
void steppersEnable(bool argEnable) {
    // EN line is active LOW on stepsticks and the 595 is not inverted on this board, so we invert argEnable
    srSetBit(SR_STEPPER_DISABLE_BIT, !argEnable);

    //If we are disabling stepper motors, we lose the home position on both stepper motors
    if (!argEnable) {
        hasHomePositionAlt = false;
        hasHomePositionAz = false;
        trackingRADecTarget = false;
    }
}

//Disable stepper motors and stop all further code execution
void steppersDisableAndAbortAllCode() {
    steppersEnable(false);
    while (true) {
        delay(1000);
    }
}

//Sets the current position of this stepper motor to be angle supplied. Does not move the stepper.
bool stepperMotorSetCurrentPositionToAngle(AccelStepper &stepper, float argAngle, bool moveInPositiveDirection) {
    int8_t movementDirectionMultiplier = 0;
    if (moveInPositiveDirection) {
        movementDirectionMultiplier = 1;
    } else {
        movementDirectionMultiplier = -1;
    }

    steppersEnable(true); //Hold stepper motor in place

    long currentPositioninSteps = (STEPPER_STEPS_PER_REV / 360.0) * argAngle * movementDirectionMultiplier;
    stepper.setCurrentPosition(currentPositioninSteps);

    // Return true if the current position of the stepper motor now matches what we set it to
    return stepper.currentPosition() == currentPositioninSteps;
}

//Sets the current position of the az motor to be angle supplied. Does not move the stepper. Sets hasHomePositionAz to true if setting was successful
void azStepperMotorSetCurrentPositionToAngle(float argAngle) {
    hasHomePositionAz = stepperMotorSetCurrentPositionToAngle(stepperAz, argAngle, STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION);
}

//Home the stepper motor given
bool stepperMotorHome(AccelStepper &stepper, uint8_t homeLimitSwitchPin, uint8_t endLimitSwitchPin, bool moveInPositiveDirection) {
    int8_t movementDirectionMultiplier = 0;
    if (moveInPositiveDirection) {
        movementDirectionMultiplier = 1;
    } else {
        movementDirectionMultiplier = -1;
    }
    OLED_print("Beggining home", INFO);

    //Stop stepper and set current position to 0
    steppersEnable(true);
    stepper.stop();
    stepper.setCurrentPosition(0);

    //Move off home limit switch
    if (digitalRead(homeLimitSwitchPin) == LOW) {
        OLED_print("Home limit switch depressed, so we will attempt to move off it", INFO);

        //Move off home limit switch
        long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH * movementDirectionMultiplier;

        stepper.moveTo(targetSteps);
        while (stepper.distanceToGo() != 0) {
            stepper.run();
        }

        //If we are still on the limit switch, abort
        if (digitalRead(homeLimitSwitchPin) == LOW) {
            OLED_print("Couldn't move off home limit switch. Aborting", ERROR);
            steppersDisableAndAbortAllCode();
        }
    }

    //Move off end limit switch
    if (digitalRead(endLimitSwitchPin) == LOW) {
        OLED_print("End limit switch depressed, so we will attempt to move off it", INFO);

        //Move off end limit switch
        long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * -ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH * movementDirectionMultiplier;

        stepper.moveTo(targetSteps);
        while (stepper.distanceToGo() != 0) {
            stepper.run();
        }

        //If we are still on the limit switch, abort
        if (digitalRead(endLimitSwitchPin) == LOW) {
            OLED_print("Couldn't move off home limit switch. Aborting", ERROR);
            steppersDisableAndAbortAllCode();
        }
    }

    //Perform Fast Home
    stepper.setSpeed(-2000 * movementDirectionMultiplier);
    while (digitalRead(homeLimitSwitchPin) == HIGH) {
        stepper.runSpeed();
        if (digitalRead(endLimitSwitchPin) == LOW) {
            OLED_print("Error: While homing, another limit switch was pressed that we didn't expect. Aborting", ERROR);
            steppersDisableAndAbortAllCode();
        }
    }

    //Stop stepper and set current position to 0
    stepper.stop();
    stepper.setCurrentPosition(0);

    //Move off home limit switch
    if (digitalRead(homeLimitSwitchPin) == LOW) {
        OLED_print("Home limit switch depressed, so we will attempt to move off it", INFO);

        //Move off home limit switch
        long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH * movementDirectionMultiplier;

        stepper.moveTo(targetSteps);
        while (stepper.distanceToGo() != 0) {
            stepper.run();
        }

        //If we are still on the limit switch, abort
        if (digitalRead(homeLimitSwitchPin) == LOW) {
            OLED_print("Couldn't move off home limit switch. Aborting", ERROR);
            steppersDisableAndAbortAllCode();
        }
    }

    //Perform Slow Home
    stepper.setSpeed(-250 * movementDirectionMultiplier);
    while (digitalRead(homeLimitSwitchPin) == HIGH) {
        stepper.runSpeed();
        if (digitalRead(endLimitSwitchPin) == LOW) {
            OLED_print("Error: While homing, another limit switch was pressed that we didn't expect. Aborting", ERROR);
            steppersDisableAndAbortAllCode();
        }
    }

    stepper.stop();
    stepper.setCurrentPosition(0);

    //Move off home limit switch
    long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * ANGLE_TO_MOVE_OFF_DEPRESSED_LIMIT_SWITCH * movementDirectionMultiplier;

    stepper.moveTo(targetSteps);
    while (stepper.distanceToGo() != 0) {
        stepper.run();
    }

    OLED_print("Homing complete", INFO);
    return true;
}

// Home the Altitude axis
bool performAltHome() {
    return stepperMotorHome(stepperAlt, PIN_ALT_LIMIT_HOME, PIN_ALT_LIMIT_END, STEPPER_ALT_MOVE_IN_POSITIVE_DIRECTION);
}

// Home the Azimuth axis
bool performAzHome() {
    //return stepperMotorHome(stepperAz, PIN_AZ_LIMIT_HOME, PIN_AZ_LIMIT_END, STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION);
    azStepperMotorSetCurrentPositionToAngle(180); //temp, wip
    return true;
}

// Perform home on each axis if needed
void performHomeOnlyIfNeededAndSetHasHome() {
    if (!hasHomePositionAlt) {
        hasHomePositionAlt = performAltHome();
    }
    if (!hasHomePositionAz) {
        hasHomePositionAz = performAzHome();
    }
}

long convertAngleToTargetSteps(float targetAngle, bool moveInPositiveDirection) {
    int8_t movementDirectionMultiplier = 0;
    if (moveInPositiveDirection) {
        movementDirectionMultiplier = 1;
    } else {
        movementDirectionMultiplier = -1;
    }

    //Calculate the target steps from the angle
    long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * targetAngle * movementDirectionMultiplier;
    return targetSteps;
}

// Moves stepper motor given to an angle. Returns true if successful, false if not
bool stepperMotorMoveToAngleFromHome(AccelStepper &stepper, uint8_t homeLimitSwitchPin, uint8_t endLimitSwitchPin, float minimumAngle, float maximumAngle, float targetAngle, bool moveInPositiveDirection) {
    int8_t movementDirectionMultiplier = 0;
    if (moveInPositiveDirection) {
        movementDirectionMultiplier = 1;
    } else {
        movementDirectionMultiplier = -1;
    }

    //Calculate the target steps from the angle
    long targetSteps = (STEPPER_STEPS_PER_REV / 360.0) * targetAngle * movementDirectionMultiplier;

    //OLED_print("Moving to angle from home: " . targetAngle . "    Target Steps:" . targetSteps, INFO);

    //Check if targetAngle is within allowed range
    if (fabs(targetAngle) < minimumAngle) {
        OLED_print("Warning: Angle requested is smaller than minimum angle", WARNING);
        return false;
    }
    if (fabs(targetAngle) > maximumAngle) {
        OLED_print("Warning: Angle requested is greater than maximum angle", WARNING);
        return false;
    }

    //Request move to steps position
    stepper.moveTo(targetSteps);

    //Create currentAngle variable
    float currentAngle = 0.0;

    // Run with acceleration
    while (stepper.distanceToGo() != 0) {
        currentAngle = (stepper.currentPosition() / STEPPER_STEPS_PER_REV) * 360.0; //We multiply by the bigger number first, then divide by 360

        if (fabs(currentAngle) - ANGLE_ALLOWED_WITHIN_LIMIT_SWITCH_BEING_DEPRESSED > minimumAngle && digitalRead(homeLimitSwitchPin) == LOW) {
            //OLED_print("The home switch was depressed while trying to move to angle that shouldn't hit that limit. currentAngle: " . currentAngle, WARNING);
            return false;
        }
        if (fabs(currentAngle) + ANGLE_ALLOWED_WITHIN_LIMIT_SWITCH_BEING_DEPRESSED < maximumAngle && digitalRead(endLimitSwitchPin) == LOW) {
            //OLED_print(("The end switch was depressed while trying to move to angle that shouldn't hit that limit. currentPosition: " . stepper.currentPosition() . "    currentAngle: " . currentAngle, WARNING);
            return false;
        }
        stepper.run();
        //steppers.runSpeedToPosition();  // Alternative option that accelerates instantly?
    }
    return true;
}

//Altered for float and double values. Originally from arduino source code
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) { //Args are: value, fromLow, fromHigh, toLow, toHigh
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max) { //Args are: value, fromLow, fromHigh, toLow, toHigh
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//Get position of the stepper from home. Should always be positive
double positionGetAngleFromHome(AccelStepper &stepper) {
    float stepperAngle = fabs((stepper.currentPosition() / STEPPER_STEPS_PER_REV) * 360.0);
    return stepperAngle;
}

//Get angle of altitude. Is allowed to be negative, means currently pointing below horizon
double positionGetAngleAlt() {
    double altAngleFromHome = positionGetAngleFromHome(stepperAlt);
    double altAngle = mapDouble(altAngleFromHome, Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME, Z_STEPPER_ANGLE_OF_ZENITH_FROM_HOME, 0.0, 90.0); //Convert the range of Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME - Z_STEPPER_ANGLE_OF_ZENITH_FROM_HOME from home to be 0-90 degrees altitude nominal, but allowed to be negative. Based on distance from home to the horizon. Zenith is used here, but in most cases it will always be Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME + 90
    altAngle = altAngle - angleOffsetAlt; //Take away the user's custom offset value
    return altAngle;
}

//Get angle of azimuth
double positionGetAngleAz() {
    double azAngleFromHome = positionGetAngleFromHome(stepperAz);
    double azAngle = mapFloat(azAngleFromHome, X_STEPPER_ANGLE_OF_NORTH_FROM_HOME, X_STEPPER_ANGLE_OF_SOUTH_FROM_HOME, 0.0, 180.0); //Convert the range of X_STEPPER_ANGLE_OF_NORTH_FROM_HOME - X_STEPPER_ANGLE_OF_SOUTH_FROM_HOME from home to be 0-180 degrees azimuth nominal, but allowed to be negative. Based on distance from home to north. South is used here, but in most cases it will always be X_STEPPER_ANGLE_OF_NORTH_FROM_HOME + 180
    azAngle = azAngle - angleOffsetAz; //Take away the user's custom offset value
    return azAngle;
}

//Get HMS Right Ascension of current position
structHoursMinutesSeconds positionGetHMSRA() {
    structRADec currentRADec = altAzToRaDecJNow(positionGetAngleAlt(), positionGetAngleAz(), latitudeDecimal, longitudeDecimal, rtcESP32Time.getYear(), rtcESP32Time.getMonth()+1, rtcESP32Time.getDay(), rtcESP32Time.getHour(true), rtcESP32Time.getMinute(), rtcESP32Time.getSecond());
    return currentRADec.rightAscension;
}

//Get DMS Declination of current position
structDegreesMinutesSeconds positionGetDMSDec() {
    structRADec currentRADec = altAzToRaDecJNow(positionGetAngleAlt(), positionGetAngleAz(), latitudeDecimal, longitudeDecimal, rtcESP32Time.getYear(), rtcESP32Time.getMonth()+1, rtcESP32Time.getDay(), rtcESP32Time.getHour(true), rtcESP32Time.getMinute(), rtcESP32Time.getSecond());
    return currentRADec.declination;
}

bool positionSetTargetAngleAltitude(float altAngle) {
    //WIP fir printing to OLED
    //snprintf (buff, sizeof(buff), "%f", val);
    //
    //val_int = (int) val;   // compute the integer part of the float
    //val_fra = (int) ((val - (float)val_int) * 1000);   // compute 3 decimal places (and convert it to int)
    //snprintf (buff, sizeof(buff), "%d.%d", val_int, val_fra); //
    //Serial.println(val);

    //dtostrf(val, 4, 6, buff);  //4 is mininum width, 6 is precision

    //OLED_print("Moving to Altitude: " . altAngle, INFO);

    float targetAngle = mapFloat(altAngle, 0.0, 90.0, Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME, Z_STEPPER_ANGLE_OF_ZENITH_FROM_HOME); //Convert the range of 0-90 from home to be 0-90 degrees altitude, based on distance from home to the horizon. Zenith is used here, but in most cases it will always be Z_STEPPER_ANGLE_OF_HORIZON_FROM_HOME + 90
    targetAngle = targetAngle + angleOffsetAlt; //Add the user's custom offset value

    //stepperMotorMoveToAngleFromHome() will check if the angle requested is save to move to
    //return stepperMotorMoveToAngleFromHome(stepperAlt, PIN_ALT_LIMIT_HOME, PIN_ALT_LIMIT_END, Z_STEPPER_MINIMUM_ANGLE, Z_STEPPER_MAXIMUM_ANGLE, targetAngle, STEPPER_ALT_MOVE_IN_POSITIVE_DIRECTION);

    //Check if targetAngle is within allowed range
    if (fabs(targetAngle) < Z_STEPPER_MINIMUM_ANGLE) {
        OLED_print("Warning: Angle requested is smaller than minimum angle", WARNING);
    } else if (fabs(targetAngle) > Z_STEPPER_MAXIMUM_ANGLE) {
        OLED_print("Warning: Angle requested is greater than maximum angle", WARNING);
    } else {
        stepperAltTargetSteps = convertAngleToTargetSteps(targetAngle, STEPPER_ALT_MOVE_IN_POSITIVE_DIRECTION);
        moveSteppersToTargetStepsAlt = true;
        return true;
    }
    return false;
}

bool positionSetTargetAngleAzimuth(float azAngle) {
    float targetAngle = mapFloat(azAngle, 0.0, 180.0, X_STEPPER_ANGLE_OF_NORTH_FROM_HOME, X_STEPPER_ANGLE_OF_SOUTH_FROM_HOME); //Convert the range of 0-180 from home to be 0-180 degrees azimuth, based on distance from home to north. South is used here, but in most cases it will always be X_STEPPER_ANGLE_OF_NORTH_FROM_HOME + 180
    targetAngle = targetAngle + angleOffsetAz; //Add the user's custom offset value

    //stepperMotorMoveToAngleFromHome() will check if the angle requested is save to move to
    //return stepperMotorMoveToAngleFromHome(stepperAz, PIN_AZ_LIMIT_HOME, PIN_AZ_LIMIT_END, X_STEPPER_MINIMUM_ANGLE, X_STEPPER_MAXIMUM_ANGLE, targetAngle, STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION);

    //Check if targetAngle is within allowed range
    if (fabs(targetAngle) < X_STEPPER_MINIMUM_ANGLE) {
        OLED_print("Warning: Angle requested is smaller than minimum angle", WARNING);
    } else if (fabs(targetAngle) > X_STEPPER_MAXIMUM_ANGLE) {
        OLED_print("Warning: Angle requested is greater than maximum angle", WARNING);
    } else {
        stepperAzTargetSteps = convertAngleToTargetSteps(targetAngle, STEPPER_AZ_MOVE_IN_POSITIVE_DIRECTION);
        moveSteppersToTargetStepsAz = true;
        return true;
    }
    return false;
}

bool positionSetTargetAltAz(structAltAz altAzAngle) {
    return positionSetTargetAngleAltitude(altAzAngle.altitudeDeg) && positionSetTargetAngleAzimuth(altAzAngle.azimuthDeg);
}

bool positionSetTargetAltAz(float altAngle, float azAngle) {
    return positionSetTargetAngleAltitude(altAngle) && positionSetTargetAngleAzimuth(azAngle);
}

//Stop stepper motors in their current position
bool steppersHaltMovement() {
    moveSteppersToTargetStepsAlt = false;
    moveSteppersToTargetStepsAz = false;
    trackingRADecTarget = false;
    bool boolReturn = positionSetTargetAngleAltitude(positionGetAngleAlt()) && positionSetTargetAngleAzimuth(positionGetAngleAz());
    return boolReturn;
}

bool positionSetTargetRADec(structHoursMinutesSeconds ra, structDegreesMinutesSeconds dec) {
    structAltAz moveToAltAzValue = raDecToAltAzJNow(ra, dec, latitudeDecimal, longitudeDecimal, rtcESP32Time.getYear(), rtcESP32Time.getMonth()+1, rtcESP32Time.getDay(), rtcESP32Time.getHour(true), rtcESP32Time.getMinute(), rtcESP32Time.getSecond());
    LX200TargetAlt = moveToAltAzValue.altitudeDeg;
    LX200TargetAz = moveToAltAzValue.azimuthDeg;
    bool boolReturn = positionSetTargetAltAz(LX200TargetAlt, LX200TargetAz);
    return boolReturn;
}
