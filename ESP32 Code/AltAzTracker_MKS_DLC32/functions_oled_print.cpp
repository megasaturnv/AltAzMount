#include "global_variables.h"

// #######################
// # OLED print function #
// #######################
//Print information to OLED
void OLED_print(const char* argString, debugLevel argDebugLevel) {
    bool doPrint = false;
    if (
        setting_OledPrintDebugLevel == INFO
        ||
        setting_OledPrintDebugLevel == WARNING && (argDebugLevel == WARNING || argDebugLevel == ERROR)
        ||
        setting_OledPrintDebugLevel == ERROR && argDebugLevel == ERROR
    ) {
        doPrint = true;
    }

    if (doPrint) {
        char debugLevelString[] = "Ukn:";
        switch (argDebugLevel) {
            case INFO:
                strncpy(debugLevelString, "Inf:", 4);
                break;
            case WARNING:
                strncpy(debugLevelString, "Wrn:", 4);
                break;
            case ERROR:
                strncpy(debugLevelString, "Err:", 4);
                break;
        }

        char combinedString[20] = "";
        strncat(combinedString, debugLevelString, sizeof(combinedString) - strlen(combinedString) - 1); // append debugLevelString safely
        strncat(combinedString, argString, sizeof(combinedString) - strlen(combinedString) - 1); // append argString safely

        oled.setCursor(0, 0);
        oled.print(combinedString);
        //delay(500); //Keep the message on screen for a set time, though this does halt execution
    }
}
