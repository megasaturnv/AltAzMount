#include <Arduino.h>

// #########################
// # LX200 Serial Commands #
// #########################
#include "functions_LX200.h"
#include "global_variables.h"
#include "functions_oled_print.h"
#include "custom_AccelStepper_overwrites.h"
#include "functions_movement.h"
#include "functions_AltAz_RADec_conversion.h"

//https://github.com/Stellarium/stellarium/blob/master/plugins/TelescopeControl/src/Lx200/Lx200Command.cpp
//https://github.com/indilib/indi/blob/master/drivers/telescope/lx200driver.cpp
/*Movement Commands
 *   :MS#: Move to Object (slews to pre-loaded RA/Dec).
 *   :Mn#, :Ms#, :Me#, :Mw#: Move North, South, East, West at slew rate.
 *   :Q#: Quit/Stop all movement.
 *   :Qn#, :Qs#, :Qe#, :Qw#: Quit movement in specific direction.
 *   :MA#: Move to Axis (Landmark).
 *   :RS#: Slew at Siderial Rate.
 * Information/Get Commands
 *   :GD#: Get Declination (target or current).
 *   :GR#: Get Right Ascension.
 *   :GA#: Get Altitude.
 *   :GZ#: Get Azimuth.
 *   :GL#: Get Local Time (24-hour).
 *   :GVD#: Get Firmware Date.
 *   :GVP#: Get Product Name.
 *   :GT#: Get Tracking Rate.
 * Setting Commands
 *   :Sr HH:MM:SS#: Set RA Target.
 *   :Sd sDDMM#: Set Declination Target.
 *   :Sg DDDMM#: Set Longitude.
 *   :St sDD*MM#: Set Latitude.
 *   :SC MM/DD/YY#: Set Calendar Date.
 *   :SL HH:MM:SS#: Set Local Time.
 * Control and System Commands
 *   :hC#: Calibrate Home Position (LX200GPS/RCX400).
 *   :hF#: Find Home Position.
 *   :CM#: Sync to Object (syncs scope to target).
 *   :AA#: Set Alt/Az Mode.
 *   :AP#: Set Polar Mode.
 *   :U#: Toggle precision between high/low. [1, 2, 3, 4, 5, 6, 7, 8]
 */


//Key commands include #:GR# (Get RA), #:GD# (Get Dec), :Sr (Set RA), :Sd (Set Dec), and :MS (Move to Selected Target)
//1. Get Right Ascension (:GR#)Sent by Stellarium: :GR#Expected Response: HH:MM:SS# (High Precision) or HH:MM.T# (Low Precision)Example String: 18:36:56#Note: Stellarium usually triggers high precision if possible.
//2. Get Declination (:GD#)Sent by Stellarium: :GD#Expected Response: sDD*MM:SS# (High Precision) or sDD*MM# (Low Precision)Example String: +38*47:01#Note: The * character represents the degree symbol (ASCII 223).
//3. Set Target Right Ascension (:Sr)Sent by Stellarium: :SrHH:MM:SS#Example String: :Sr18:36:56#Expected Response: 1 (if accepted) or 0 (if invalid).Note: Stellarium's telescope plugin defaults to this Long Format for coordinates.
//4. Set Target Declination (:Sd)Sent by Stellarium: :SdsDD*MM:SS#Example String: :Sd+38*47:01#Expected Response: 1 (if accepted) or 0 (if invalid).
//     When sending target coordinates, specifically for Declination (:Sd), Stellarium uses the standard LX200 degree symbol separator.Example: :Sd+38*47:01#The * character is the ASCII value 223 (degree symbol)
//5. Move to Selected Target (:MS#)Sent by Stellarium: :MS#Expected Response:0 if the slew can be completed.1<message># if the object is below the horizon.2<message># if the object is below a "higher" limit (e.g., restricted by the mount).Example String: 0Quick Troubleshooting TipsPrecision Toggle: Stellarium may send :U# to toggle between precision modes if it isn't getting the expected format back.

//No new lines should be sent or received over LX200

//Receive data for LX200 telescope. Begins with :, ends with #
void serialRecieveLX200() {
    static bool recvInProgress = false;
    static uint8_t index = 0;
    char startMarker = ':'; //Note the single quotes and not double quotes here
    char endMarker = '#'; //Note the single quotes and not double quotes here
    char readChar;

    while (Serial.available() > 0 && serialNewData == false) {
        readChar = Serial.read();
        if (recvInProgress == true) {
            if (readChar != endMarker) {
                serialReceivedChars[index] = readChar;
                index++;
                if (index >= serialNumChars) {
                    index = serialNumChars - 1;
                }
            } else {
                serialReceivedChars[index] = '\0'; // terminate the string
                recvInProgress = false;
                index = 0;
                serialNewData = true;
            }
        } else if (readChar == startMarker) {
            recvInProgress = true;
        }
    }
}

void serialClearBuffer() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

void serialParseData() {
    OLED_print(serialReceivedChars, INFO);
    serialNewData = false;

    if (strncmp(serialReceivedChars, "DEBUG", 20) == 0) {
        // Print debug info
        Serial.println();
        Serial.println("Has home:");
        Serial.println(hasHomePositionAlt);
        Serial.println(hasHomePositionAz);
        Serial.println("Targets:");
        Serial.println(LX200TargetAlt);
        Serial.println(LX200TargetAz);
        Serial.println(moveSteppersToTargetStepsAlt);
        Serial.println(moveSteppersToTargetStepsAz);
        Serial.println(stepperAltTargetSteps);
        Serial.println(stepperAzTargetSteps);
        Serial.println("Stepper distance to go:");
        Serial.println(stepperAlt.distanceToGo());
        Serial.println(stepperAz.distanceToGo());
        Serial.println("trackingRADecTarget:");
        Serial.println(trackingRADecTarget);
        Serial.println();
    } else if (strncmp(serialReceivedChars, "Q", 20) == 0) {
        // Halt all current slewing
        // Returns:Nothing
        steppersHaltMovement();

    } else if (strncmp(serialReceivedChars, "U", 20) == 0) {
        // Toggle between low/hi precision positions
        // Low - RA displays and messages HH:MM.T sDD*MM
        // High - Dec/Az/El displays and messages HH:MM:SS sDD*MM:SS
        // Returns Nothing
        LX200HighPrecisionMode = !LX200HighPrecisionMode;

    } else if (strncmp(serialReceivedChars, "D", 20) == 0) {
        // Requests a string of bars indicating the distance to the current library object.
        // Returns:
        // LX200's – a string of bar characters indicating the distance.
        // Autostars and LX200GPS – a string containing one bar until a slew is complete, then a null string is returned
        if (abs(stepperAlt.distanceToGo()) < WITHIN_STEPS_TO_ARRIVE_AT_TARGET && abs(stepperAz.distanceToGo()) < WITHIN_STEPS_TO_ARRIVE_AT_TARGET) {
            Serial.write(0); //Write null character to say we have arrived at the target
        } else {
            Serial.print("#"); //Write "bar" character to say we have not yet arrived at the target. Documentation isn't clear what this character is, unless it's "bar" in the hitachi character set
        }

    } else if (strncmp(serialReceivedChars, "Gl", 20) == 0) {
        // Custom response, get light level from LDR

        int randomValue = random(0, 256);
        if (randomValue < 100) Serial.print('0');
        if (randomValue < 10)  Serial.print('0');
        Serial.print(randomValue);
        Serial.print("#");

    } else if (strncmp(serialReceivedChars, "GA", 20) == 0) {
        // Get Telescope Altitude
        // Returns: sDD*MM# or sDD*MM’SS#
        // The current scope altitude. The returned format depending on the current precision setting.
        performHomeOnlyIfNeededAndSetHasHome();
        structDegreesMinutesSeconds currentAltDMS = decimalDegreesToDMS(positionGetAngleAlt());

        if (LX200HighPrecisionMode) {
            // High precision mode
            Serial.printf("%+03d*%02d'%02.0f#", currentAltDMS.degrees, currentAltDMS.minutes, currentAltDMS.seconds);
            //Serial.printf("%+03d\xDF%02d:%02.0f#", currentAltDMS.degrees, currentAltDMS.minutes, currentAltDMS.seconds);
        } else {
            // Low precision mode
            Serial.printf("%+03d*%02d#", currentAltDMS.degrees, currentAltDMS.minutes);
            //Serial.printf("%+03d\xDF%02d#", currentAltDMS.degrees, currentAltDMS.minutes);
        }

    } else if (strncmp(serialReceivedChars, "GZ", 20) == 0) {
        // Get telescope azimuth
        // Returns: DDD*MM#T or DDD*MM’SS#
        // The current telescope Azimuth depending on the selected precision.
        performHomeOnlyIfNeededAndSetHasHome();
        structDegreesMinutesSeconds currentAzDMS = decimalDegreesToDMS(positionGetAngleAz());

        if (LX200HighPrecisionMode) {
            // High precision mode
            Serial.printf("%03d*%02d'%02.0f#", currentAzDMS.degrees, currentAzDMS.minutes, currentAzDMS.seconds);
            //Serial.printf("%03d\xDF%02d:%02.0f#", currentAzDMS.degrees, currentAzDMS.minutes, currentAzDMS.seconds);
        } else {
            // Low precision mode
            Serial.printf("%03d*%02d#", currentAzDMS.degrees, currentAzDMS.minutes);
            //Serial.printf("%03d\xDF%02d#", currentAzDMS.degrees, currentAzDMS.minutes);
        }

    } else if (strncmp(serialReceivedChars, "Sa", 2) == 0) {
        // Set target object altitude to sDD*MM# or sDD*MM’SS# [LX 16”, Autostar, Autostar II]
        // Returns:
        // 1 - Object within slew range
        // 0 - Object out of slew range
        trackingRADecTarget = false;

        bool setSuccessful = false;
        int parsedCount, parsedDegrees, parsedMinutes, parsedSeconds;
        structDegreesMinutesSeconds parsedDMS;
        if (LX200HighPrecisionMode) {
            parsedCount = sscanf(serialReceivedChars, "Sa%2d*%2d'%2d", &parsedDegrees, &parsedMinutes, &parsedSeconds);
            //parsedCount = sscanf(serialReceivedChars, "Sa%2d\xDF%2d:%2d", &parsedDegrees, &parsedMinutes, &parsedSeconds);
            if (parsedCount == 3) {
                parsedDMS.degrees = parsedDegrees;
                parsedDMS.minutes = parsedMinutes;
                parsedDMS.seconds = parsedSeconds;
                LX200TargetAlt = dmsToDegrees(parsedDMS);
                setSuccessful = true;
            } else {
                OLED_print("Cant parse Sa HP", WARNING);
            }
        } else {
            parsedCount = sscanf(serialReceivedChars, "Sa%3d*%2d", &parsedDegrees, &parsedMinutes);
            //parsedCount = sscanf(serialReceivedChars, "Sa%3d\xDF%2d", &parsedDegrees, &parsedMinutes);
            if (parsedCount == 2) {
                parsedDMS.degrees = parsedDegrees;
                parsedDMS.minutes = parsedMinutes;
                parsedDMS.seconds = 0;
                LX200TargetAlt = dmsToDegrees(parsedDMS);
                setSuccessful = true;
            } else {
                OLED_print("Cant parse Sa LP", WARNING);
            }
        }
        if (setSuccessful) { // We could print setSuccessful directly, but this has more control
            Serial.print("1");
        } else {
            Serial.print("0");
        }

    } else if (strncmp(serialReceivedChars, "Sz", 2) == 0) {
        // :SzDDD*MM#
        // Sets the target Object Azimuth [LX 16” and Autostar II only]
        // Returns:
        // 0 – Invalid
        // 1 - Valid
        trackingRADecTarget = false;

        bool setSuccessful = false;
        int parsedCount, parsedDegrees, parsedMinutes;
        structDegreesMinutesSeconds parsedDMS;
        parsedCount = sscanf(serialReceivedChars, "Sz%3d*%2d", &parsedDegrees, &parsedMinutes);
        //parsedCount = sscanf(serialReceivedChars, "Sz%3d\xDF%2d", &parsedDegrees, &parsedMinutes);
        if (parsedCount == 2) {
            parsedDMS.degrees = parsedDegrees;
            parsedDMS.minutes = parsedMinutes;
            parsedDMS.seconds = 0;
            LX200TargetAz = dmsToDegrees(parsedDMS);
            setSuccessful = true;
        } else {
            OLED_print("Cant parse Sz", WARNING);
        }
        if (setSuccessful) { // We could print moveSucessful directly, but this has more control
            Serial.print("1");
        } else {
            Serial.print("0");
        }

    } else if (strncmp(serialReceivedChars, "MA", 20) == 0) {
        // :MA#
        // Autostar, LX 16”, LX200GPS – Slew to target Alt and Az
        // Returns:
        // 0 - No fault
        // 1 – Fault
        // LX200 – Not supported
        bool moveSuccessful = false;
        moveSuccessful = positionSetTargetAltAz(LX200TargetAlt, LX200TargetAz);

        if (moveSuccessful) { // We could print !moveSucessful directly, but this has more control
            Serial.print("0"); //Todo, implement returning 1 if object below horizon. Manual also says return 2 if "Object Below Higher", whatever that means...
        } else {
            Serial.print("1");
        }

    } else if (strncmp(serialReceivedChars, "GR", 20) == 0) {
        // Get Telescope RA
        // Returns: HH:MM.T# or HH:MM:SS#
        // Depending which precision is set for the telescope

        //Might be able to do decimal seconds for high precision e.g. HH:MM:SS.S

        performHomeOnlyIfNeededAndSetHasHome();
        structHoursMinutesSeconds currentRAHMS = positionGetHMSRA();

        if (LX200HighPrecisionMode) {
            // High precision mode
            Serial.printf("%02d:%02d:%02.0f#", currentRAHMS.hours, currentRAHMS.minutes, currentRAHMS.seconds);
        } else {
            // Low precision mode
            Serial.printf("%02d:%02d.%1d#", currentRAHMS.hours, currentRAHMS.minutes, floor(currentRAHMS.seconds / 6));
        }

    } else if (strncmp(serialReceivedChars, "GD", 20) == 0) {
        // Get Telescope Declination.
        // Returns: sDD*MM# or sDD*MM’SS#
        // Depending upon the current precision setting for the telescope.

        // Some say degrees symbol 0xDF is needed (This matches the character set used on the Hitachi HD44780 LCD controller) rather than *, ' or ’
        // "Extended Protocol" is sDD*MM:SS#

        performHomeOnlyIfNeededAndSetHasHome();
        structDegreesMinutesSeconds currentDecDMS = positionGetDMSDec();

        if (LX200HighPrecisionMode) {
            // High precision mode
            //Serial.printf("%+03d*%02d'%02.0f#", currentDecDMS.degrees, currentDecDMS.minutes, currentDecDMS.seconds);
            Serial.printf("%+03d\xDF%02d:%02.0f#", currentDecDMS.degrees, currentDecDMS.minutes, currentDecDMS.seconds);
        } else {
            // Low precision mode
            //Serial.printf("%+03d*%02d#", currentDecDMS.degrees, currentDecDMS.minutes);
            Serial.printf("%+03d\xDF%02d#", currentDecDMS.degrees, currentDecDMS.minutes);
        }

    } else if (strncmp(serialReceivedChars, "Sr", 2) == 0) {
        // Set target object RA to HH:MM.T or HH:MM:SS depending on the current precision setting.
        // Returns:
        // 0 – Invalid
        // 1 - Valid
        int parsedCount, parsedHours, parsedMinutes, parsedSeconds;
        if (LX200HighPrecisionMode) {
            parsedCount = sscanf(serialReceivedChars, "Sr%2d:%2d:%2d", &parsedHours, &parsedMinutes, &parsedSeconds);
            if (parsedCount == 3) {
                LX200TargetRA.hours = parsedHours;
                LX200TargetRA.minutes = parsedMinutes;
                LX200TargetRA.seconds = parsedSeconds; //todo, extract float from serial string
                Serial.print("1"); // Todo, implement check if request is within slew range
            } else {
                OLED_print("Cant parse Sr HP", WARNING);
                Serial.print("0"); // Todo, implement check if request is within slew range
            }
        } else {
            parsedCount = sscanf(serialReceivedChars, "Sr%2d:%2d.%1d", &parsedHours, &parsedMinutes, &parsedSeconds);
            if (parsedCount == 3) {
                parsedSeconds = parsedSeconds * 6; //In low precision mode, seconds place is actually 10ths of a minute so we need to convert to seconds
                LX200TargetRA.hours = parsedHours;
                LX200TargetRA.minutes = parsedMinutes;
                LX200TargetRA.seconds = parsedSeconds; //todo, extract float from serial string
                Serial.print("1"); // Todo, implement check if request is within slew range
            } else {
                OLED_print("Cant parse Sr LP", WARNING);
                Serial.print("0"); // Todo, implement check if request is within slew range
            }
        }

    } else if (strncmp(serialReceivedChars, "Sd", 2) == 0) {
        // Set target object declination to sDD*MM or sDD*MM:SS depending on the current precision setting
        // Returns:
        // 1 - Dec Accepted
        // 0 – Dec invalid

        // Not consistent if it should be sDD*MM:SS or sDD*MM'SS in the LX200 protocol documentation
        // Stellarium sends :SdsDD.MM:SS#, which is completely different to documentation. First dot is a degree symbol 0xDF

        int parsedCount, parsedDegrees, parsedMinutes, parsedSeconds;
        if (LX200HighPrecisionMode) {
            parsedCount = sscanf(serialReceivedChars, "Sd%3d\xDF%2d:%2d", &parsedDegrees, &parsedMinutes, &parsedSeconds); // Note %03d on the first number due to +/- symbol
            if (parsedCount == 3) {
                LX200TargetDec.degrees = parsedDegrees;
                LX200TargetDec.minutes = parsedMinutes;
                LX200TargetDec.seconds = parsedSeconds; //todo, extract float from serial string
                Serial.print("1"); // Todo, implement check if request is within slew range
            } else {
                OLED_print("Cant parse Sd HP", WARNING);
                Serial.print("0"); // Todo, implement check if request is within slew range
            }
        } else {
            parsedCount = sscanf(serialReceivedChars, "Sd%3d\xDF%2d", &parsedDegrees, &parsedMinutes); // Note %03d on the first number due to +/- symbol
            if (parsedCount == 2) {
                parsedSeconds = 0;
                LX200TargetDec.degrees = parsedDegrees;
                LX200TargetDec.minutes = parsedMinutes;
                LX200TargetDec.seconds = parsedSeconds; //todo, extract float from serial string
                Serial.print("1"); // Todo, implement check if request is within slew range
            } else {
                OLED_print("Cant parse Sd LP", WARNING);
                Serial.print("0"); // Todo, implement check if request is within slew range
            }
        }

    } else if (strncmp(serialReceivedChars, "MS", 20) == 0) {
        //Slew to Target Object
        // Returns:
        // 0
        // 1<string>#
        // 2<string>#
        // Slew is Possible
        // Object Below Horizon w/string message
        // Object Below Higher w/string message
        bool moveSuccessful = false;
        moveSuccessful = positionSetTargetRADec(LX200TargetRA, LX200TargetDec);

        trackingRADecTarget = moveSuccessful;

        if (moveSuccessful) { // We could print !moveSucessful directly, but this has more control
            Serial.print("0"); // Todo, implement returning 1 if object below horizon. Manual also says return 2 if "Object Below Higher", whatever that means...
        } else {
            Serial.print("1");
        }

    } else {
        OLED_print("Didn't understand LX200 command", WARNING);
        OLED_print(serialReceivedChars, WARNING);
        /*
        Serial.println("didn't understand LX200 command");
        Serial.print("_");
        Serial.print(serialReceivedChars);
        Serial.println("_");
        Serial.println();
        */
    }
}
