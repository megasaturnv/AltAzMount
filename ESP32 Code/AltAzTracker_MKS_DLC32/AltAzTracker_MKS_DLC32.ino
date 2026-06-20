// ###################################
// ### MKS DLC32 v2.1 AltAzTracker ###
// ###################################

// ###################
// # Define settings #
// ###################
#include "settings.h"



// ############
// # Includes #
// ############
#include <Arduino.h>

//#include <stdint.h>
//#include <stdlib.h>
//#include <absstring.h>
//#include <math.h>

// I2C OLED Devices
#include <Wire.h>

#if DISPLAY_OUTPUT == SSD1306
  #include <SSD1306Ascii.h>
  #include <SSD1306AsciiWire.h>
#elif DISPLAY_OUTPUT == SH1106
  #include <U8g2lib.h>
#endif

#include <menu.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>
#if DISPLAY_OUTPUT == SSD1306
  #include <menuIO/SSD1306AsciiOut.h>
#elif DISPLAY_OUTPUT == SH1106
  #include <menuIO/u8g2Out.h>
#endif

#include <ClickEncoder.h> // Using this library: https://github.com/soligen2010/encoder
#include <menuIO/clickEncoderIn.h>

using namespace Menu; //Copied from example code. Alternative is to use Menu::x

#if DISPLAY_OUTPUT == SSD1306
  // #define menuFont System5x7
  #define menuFont lcd5x7
  #define fontW 5
  #define fontH 8
#elif DISPLAY_OUTPUT == SH1106
  #define menuFont u8g2_font_7x13_mf
  //#define menuFont u8g2_font_6x12_tf
  #define fontW 7
  #define fontH 16
  #define offsetX 0
  #define offsetY 3
  #define U8_Width 128
  #define U8_Height 64
  #define USE_HWI2C
#endif



// ##############
// # Structures #
// ##############
#include "structures.h"



// ##################
// # Const settings #
// ##################
#include "settings.h"



// ####################
// # Global Variables #
// ####################
#include "global_variables.h"



// ############################################################
// # custom class_AltAzPreferences.h which uses preferences.h #
// ############################################################
#include "class_AltAzPreferences.h"



// ##################################
// # Functions for AltAzPreferences #
// ##################################
#include "functions_AltAzPreferences.h"



// ################
// # Menu Defines #
// ################
#include "arduino_menu.h"



// ################################
// # Functions for Shift Register #
// ################################
#include "functions_shift_register.h"



// ##########################################
// # Custom AccelStepper Library Overwrites #
// ##########################################
#include "custom_AccelStepper_overwrites.h"



// ######################################
// # Alt/Az RA/Dec conversion functions #
// ######################################
#include "functions_AltAz_RADec_conversion.h"



// #######################
// # OLED print function #
// #######################
#include "functions_oled_print.h"



// ##########################
// # Functions for movement #
// ##########################
#include "functions_movement.h"



// #######################################
// # Functions for LX200 Serial Commands #
// #######################################
#include "functions_LX200.h"



// #############################################################################
// # Functions for calculating the RA and Dec of objects like the sun and moon # 
// #############################################################################
#include "functions_Calculate_RA_Dec_Of_Object.h"



// ##################
// # Menu Structure #
// ##################
//#include "arduino_menu_structure.h"
// https://github.com/neu-rah/ArduinoMenu/wiki/Menu-definition
//FIELD Parameters: var.name, title, units, min., max., step size, fine step size, action, events mask, styles

// define menu colors --------------------------------------------------------
//each color is in the format:
//  { {disabled normal,disabled selected}, {enabled normal,enabled selected, enabled editing} }
// this is a monochromatic color table
const colorDef<uint8_t> colors[6] MEMMODE={
  {{0,0},{0,1,1}},//bgColor
  {{1,1},{1,0,0}},//fgColor
  {{1,1},{1,0,0}},//valColor
  {{1,1},{1,0,0}},//unitColor
  {{0,1},{0,0,1}},//cursorColor
  {{1,1},{1,0,0}},//titleColor
};

// # Menu events
//Return options can be proceed and quit but documentation is sparse
//You can also return button inputs like "nav.doNav(escCmd)"
// when menu is suspended
result idle(menuOut &o, idleEvent e) {
  o.clear();

  switch (e) {//FIELD Parameters: var.name, title, units, min., max., step size, fine step size, action, events mask, styles
    case idleStart:
      o.println("suspending menu!");
      steppersEnable(false);
      break;
    case idling:
      //o.println("suspended...");
      o.printf("%d %d %d %d %d %d\n", rtcESP32Time.getYear(),    rtcESP32Time.getMonth()+1,    rtcESP32Time.getDay(),    rtcESP32Time.getHour(true),   rtcESP32Time.getMinute(),    rtcESP32Time.getSecond()    );
      o.printf("%f, %f\n", latitudeDecimal, longitudeDecimal);
      break;
    case idleEnd:
      o.println("resuming menu.");
      break;
  }

  return proceed;
}

result menuEvent_updateNewDateAndTimeVariable(eventMask e) {
  setNewDateAndTime.tm_year = rtcESP32Time.getYear();
  setNewDateAndTime.tm_mon = rtcESP32Time.getMonth();
  setNewDateAndTime.tm_mday = rtcESP32Time.getDay();
  setNewDateAndTime.tm_hour = rtcESP32Time.getHour(true);
  setNewDateAndTime.tm_min = rtcESP32Time.getMinute();
  setNewDateAndTime.tm_sec = rtcESP32Time.getSecond();
  return proceed;
}

result menuEvent_updateLatitudeDecimal(eventMask e) {
  latitudeDecimal = dmsToDegrees(latitudeDMSDegrees, latitudeDMSMinutes, latitudeDMSSeconds);
  return proceed;
}

result menuEvent_updateLongitudeDecimal(eventMask e) {
  longitudeDecimal = dmsToDegrees(longitudeDMSDegrees, longitudeDMSMinutes, longitudeDMSSeconds);
  return proceed;
}

result op_setLatLong(eventMask e) {
  altAzPreferences.setLatitude(latitudeDecimal);
  altAzPreferences.setLongitude(longitudeDecimal);
  return quit;
}

result showEvent(eventMask e, navNode& nav, prompt &item) {
  return proceed;
}

result action1(eventMask e) {
  return proceed;
}

result action2(eventMask e, navNode& nav, prompt &item) {
  return quit;
}

result op_setDatetimeOnRTC(eventMask e) {
  rtcDS3231.adjust(DateTime(
    setNewDateAndTime.tm_year,
    setNewDateAndTime.tm_mon,
    setNewDateAndTime.tm_mday,
    setNewDateAndTime.tm_hour,
    setNewDateAndTime.tm_min,
    setNewDateAndTime.tm_sec
  ));
  rtcESP32Time.setTime(
    setNewDateAndTime.tm_sec,
    setNewDateAndTime.tm_min,
    setNewDateAndTime.tm_hour,
    setNewDateAndTime.tm_mday,
    setNewDateAndTime.tm_mon,
    setNewDateAndTime.tm_year
  );
  return quit;
}

result op_disableSteppers(eventMask e) {
  steppersEnable(false);
  return proceed;
}

result op_performAltHome(eventMask e) {
  hasHomePositionAlt = performAltHome();
  positionSetTargetAngleAltitude(0.0);
  return proceed;
}

result op_performAzHome(eventMask e) {
  hasHomePositionAz = performAzHome();
  return proceed;
}

result op_FactoryReset(eventMask e) {
  altAzPreferences.factoryReset();
  LoadPreferencesIntoMemory();
  return quit;
}

int azPosHomeAs = 0;
result menuEvent_azPosHomeAs(eventMask e) {
  char stringAzPosHomeAs[] = "   ";
  itoa(azPosHomeAs, stringAzPosHomeAs, 10);
  //sprintf(azPosHomeAs, "%d", stringAzPosHomeAs);
  OLED_print(stringAzPosHomeAs, INFO);

  //Stop stepper and set current position to 0
  stepperAz.stop();
  azStepperMotorSetCurrentPositionToAngle(azPosHomeAs);

  return proceed;
}

result op_setTargetToObject(eventMask e) {
  setTargetToObject("Sun");
  return proceed;
}

result op_beginTracking(eventMask e) {
  trackingRADecTarget = true;
  return proceed;
}

result op_stopTracking(eventMask e) {
  trackingRADecTarget = false;
  return proceed;
}



// # Menu items
MENU(subMenu_SetDateAndTime, "Set UTC Date And Time", menuEvent_updateNewDateAndTimeVariable, enterEvent, noStyle
  , FIELD(setNewDateAndTime.tm_year,"Year","", 2000, 2100, 10, 1, doNothing, noEvent, noStyle)
  , FIELD(setNewDateAndTime.tm_mon,"Month","", 1, 12, 10, 1, doNothing, noEvent, noStyle) //todo, use intermediate month variable which is tm_mon + 1
  , FIELD(setNewDateAndTime.tm_mday,"Day","", 1, 31, 10, 1, doNothing, noEvent, noStyle)
  , FIELD(setNewDateAndTime.tm_hour,"Hour","", 0, 23, 10, 1, doNothing, noEvent, noStyle)
  , FIELD(setNewDateAndTime.tm_min,"Minute","", 0, 59, 10, 1, doNothing, noEvent, noStyle)
  , FIELD(setNewDateAndTime.tm_sec,"Second","", 0, 59, 10, 1, doNothing, noEvent, noStyle)
  , OP("Set datetime on RTC", op_setDatetimeOnRTC, enterEvent)
  , EXIT("<Back")
);

MENU(subMenu_SetLatitude, "Set Latitude", doNothing, noEvent, noStyle
  //, FIELD(latitudeDecimal,"Lat","°", -90.0, 90.0, 10, 0.1, menuEvent_updateLatitudeDMS, enterEvent, noStyle)
  , FIELD(latitudeDecimal,"Lat","°", -90.0, 90.0, 10, 0.1, doNothing, noEvent, noStyle)
  , FIELD(latitudeDMSDegrees, "Lat Deg", "°", -90, 90, 10, 1, menuEvent_updateLatitudeDecimal, enterEvent, noStyle)
  , FIELD(latitudeDMSMinutes, "Lat Min", "'", 0, 59, 10, 1, menuEvent_updateLatitudeDecimal, enterEvent, noStyle)
  , FIELD(latitudeDMSSeconds, "Lat Sec", "\"", 0, 59, 10, 0.1, menuEvent_updateLatitudeDecimal, enterEvent, noStyle)
  , EXIT("<Back")
);

MENU(subMenu_SetLongitude, "Set Longitude", doNothing, noEvent, noStyle
  //, FIELD(longitudeDecimal,"Long","°", -90.0, 90.0, 10, 0.1, menuEvent_updateLongitudeDMS, enterEvent, noStyle)
  , FIELD(longitudeDecimal,"Long","°", -90.0, 90.0, 10, 0.1, doNothing, noEvent, noStyle)
  , FIELD(longitudeDMSDegrees, "Long Deg", "°", -90, 90, 10, 1, menuEvent_updateLongitudeDecimal, enterEvent, noStyle)
  , FIELD(longitudeDMSMinutes, "Long Min", "'", 0, 59, 10, 1, menuEvent_updateLongitudeDecimal, enterEvent, noStyle)
  , FIELD(longitudeDMSSeconds, "Long Sec", "\"", 0, 59, 10, 0.1, menuEvent_updateLongitudeDecimal, enterEvent, noStyle)
  , EXIT("<Back")
);

MENU(subMenu_SetLocation, "Set Location", doNothing, noEvent, noStyle
  , SUBMENU(subMenu_SetLatitude)
  , SUBMENU(subMenu_SetLongitude)
  , OP("Save new LatLong", op_setLatLong, enterEvent)
  , EXIT("<Back")
);

CHOOSE(azPosHomeAs, choose_azPosHomeAs, "Set Az Pos Home as", doNothing, noEvent, noStyle
  , VALUE("North here", 0, menuEvent_azPosHomeAs, enterEvent)
  , VALUE("East here", 90, menuEvent_azPosHomeAs, enterEvent)
  , VALUE("South here", 180, menuEvent_azPosHomeAs, enterEvent)
  , VALUE("West here", 270, menuEvent_azPosHomeAs, enterEvent)
);

MENU(subMenu_Movement, "Movement", doNothing, noEvent, noStyle
  , OP("Disable Steppers", op_disableSteppers, enterEvent)
  , OP("Perform Alt Home", op_performAltHome, enterEvent)
  , FIELD(angleOffsetAlt, "Alt Offset", "°", -45, 45, 1, 0.1, doNothing, noEvent, noStyle)
  , OP("Perform Az Home", op_performAzHome, enterEvent)
  , SUBMENU(choose_azPosHomeAs)
  , FIELD(angleOffsetAz, "Az Offset", "°", -45, 45, 1, 0.1, doNothing, noEvent, noStyle)
  , OP("Begin Tracking", op_beginTracking, enterEvent)
  , OP("Stop Tracking", op_stopTracking, enterEvent)
  , EXIT("<Back")
);

MENU(subMenu_PointAtObject, "Point at object", doNothing, noEvent, noStyle
  , OP("Sun", op_setTargetToObject, enterEvent)
  , OP("Moon", op_setTargetToObject, enterEvent)
  , OP("Begin Tracking", op_beginTracking, enterEvent)
  , OP("Stop Tracking", op_stopTracking, enterEvent)
  , EXIT("<Back")
);

TOGGLE(tripodModeAltAz, toggle_tripodModeAltAz, "Tripod Mode: ", doNothing, noEvent, noStyle
  , VALUE("AltAz", true, doNothing, noEvent)
  , VALUE("RADec", false, doNothing, noEvent)
);

MENU(subMenu_FactoryReset, "Factory Reset", doNothing, noEvent, noStyle
  , OP("Are you sure?", doNothing, noEvent)
  , OP("Yes", op_FactoryReset, enterEvent)
  , EXIT("No")
  , EXIT("<Back")
);

bool toggleTest = false;
TOGGLE(toggleTest, subMenu_toggleMenu, "toggleTest: ", doNothing, noEvent, noStyle //,doExit,enterEvent,noStyle
  , VALUE("True", true, doNothing, noEvent)
  , VALUE("False", false, doNothing, noEvent)
);

int selectTest = 0;
SELECT(selectTest, subMenu_selectMenu, "Select", doNothing, noEvent, noStyle
  , VALUE("Zero", 0, doNothing, noEvent)
  , VALUE("One", 1, doNothing, noEvent)
  , VALUE("Two", 2, doNothing, noEvent)
);

int chooseTest = -1;
CHOOSE(chooseTest, subMenu_chooseMenu, "Choose", doNothing, noEvent, noStyle
  , VALUE("First", 1, doNothing, noEvent)
  , VALUE("Second", 2, doNothing, noEvent)
  , VALUE("Third", 3, doNothing, noEvent)
  , VALUE("Last", -1, doNothing, noEvent)
);

MENU(mainMenu, "Main menu", doNothing, noEvent, noStyle
  , SUBMENU(subMenu_SetDateAndTime)
  , SUBMENU(subMenu_SetLocation)
  , SUBMENU(subMenu_Movement)
  , SUBMENU(subMenu_PointAtObject)
  , SUBMENU(toggle_tripodModeAltAz)
  , SUBMENU(subMenu_FactoryReset)
  , OP("OpAnyEvent Act1", action1, anyEvent)
  , OP("OpEnterEvent Act2", action2, enterEvent)
  , SUBMENU(subMenu_toggleMenu)
  , SUBMENU(subMenu_selectMenu)
  , SUBMENU(subMenu_chooseMenu)
  , EXIT("<Back")
);



// ##########################
// # Menu inputs and output #
// ##########################
//input section
// Declare the clickencoder
ClickEncoder clickEncoder = ClickEncoder(PIN_EXP2_TRA, PIN_EXP2_TRB, -1, ENCODER_SENSITIVITY); // -1 means don't pass through an input button here. That will be handled by joystickBtn_map later
ClickEncoderStream encStream(clickEncoder, 1); // Number means number of rotary steps to move menu, after sensitivity has been set properly. 1 step = move menu by 1

// Start the ESP32 timer to read the clickEncoder every 1 ms
//void IRAM_ATTR onTimer() { // ESP32 core 2.x
void ARDUINO_ISR_ATTR onTimer() { // ESP32 core 3.x
  clickEncoder.service();
}

// Build a map of keys to menu commands
// Negative pin numbers use internal pull-up, this is on when low
keyMap joystickBtn_map[] = {
  { (int8_t)(-1 * PIN_EXP2_CONFIRM), defaultNavCodes[enterCmd].ch} ,
  { (int8_t)(-1 * PIN_EXP1_BACK), defaultNavCodes[escCmd].ch}  ,
};
keyIn<TOTAL_NAV_BUTTONS> joystickBtns(joystickBtn_map); // The input driver

// Input from the encoder + joystick buttons
MENU_INPUTS(in, &encStream, &joystickBtns); //Macro shortcut

// Output section
// Define at least one panel for menu output
#if DISPLAY_OUTPUT == SSD1306
  //const panel panels[] MEMMODE = {  { &mainMenu, 0, 1, 128 / fontW, (64 / fontH)-1} }; //Start at row 1 to leave the top row available for messages. Total rows is therefore -1 to not go off-screen
  const panel panels[] MEMMODE = {  {0, 1, 128 / fontW, (64 / fontH)-1} }; //Start at row 1 to leave the top row available for messages. Total rows is therefore -1 to not go off-screen
#elif DISPLAY_OUTPUT == SH1106
  const panel panels[] MEMMODE = { {0, 1, 132 / fontW, (64 / fontH)-1} }; //Start at row 1 to leave the top row available for messages. Total rows is therefore -1 to not go off-screen
#endif

navNode* nodes[sizeof(panels) / sizeof(panel)]; // navNodes to store navigation status
panelsList pList(panels, nodes, 1); // A list of panels and nodes
idx_t tops[MAX_DEPTH] = {0, 0}; // Store cursor positions for each level

#if DISPLAY_OUTPUT == SSD1306
  SSD1306AsciiOut outOLED(&oled, tops, pList, 5, 1); //oled output device menu driver
#elif DISPLAY_OUTPUT == SH1106
  // Define geometry and font
  //const uint8_t charW = 6;
  //const uint8_t charH = 10;
  //const uint8_t screenWidthChars = 128 / charW; // ~21
  //const uint8_t screenHeightChars = 64 / charH; // ~6

  // Define the panel (x, y, width, height) in character units
  //rect myPanel(0, 0, 21, 6);
  //const panel myPanel[] MEMMODE = {
  //  {0, 0, 21, 8}
  //};
  //panelsList pList

  // Manual u8g2Out instantiation (7 arguments)
  u8g2Out outOLED(
    oled,    // display object
    colors,  // color palette
    tops,
    pList,
    fontW,   // font width
    fontH,   // font height
    offsetX, // pixel offset X
    offsetY, // pixel offset Y
    1,       // int fontMarginX
    1        // int fontMarginY
  );
#endif

menuOut* constMEM outputs[] MEMMODE  = {&outOLED}; // List of output devices
outputsList out(outputs, 1); // Outputs list

//Macro shortcut. Never tested
//MENU_OUTPUTS(out2,MAX_DEPTH,U8G2_OUT(oled,colors,fontW,fontH,offsetX,offsetY,{0,0,U8_Width/fontW,U8_Height/fontH}));

//Define navroot
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);





// Create ESP32 timer
// http://www.iotsharing.com/2017/06/how-to-use-interrupt-timer-in-arduino-esp32.html https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;







// ##############
// # Main setup #
// ##############
void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(100);
    while (!Serial);

    // Begin button end rotary encoder inputs
    joystickBtns.begin();
    //encoder.begin(); //Only needed for menu's builtin encoder lib
    clickEncoder.setAccelerationEnabled(true);
    clickEncoder.setDoubleClickEnabled(false); // Disable doubleclicks makes the response faster.  See: https://github.com/soligen2010/encoder/issues/6

    // ESP32 timer
    //timer = timerBegin(0, 80, true); //ESP32 core 2.x. Prescaler 80 means 80MHz/80 = 1MHz ticks
    // Initialize timer with frequency in Hz. This makes each "tick" 1,000,000 times per second
    timer = timerBegin(1000000); //ESP32 core 3.x

    //timerAttachInterrupt(timer, &onTimer, true); //ESP32 core 2.x
    // Attach the ISR function to the timer
    timerAttachInterrupt(timer, &onTimer); //ESP32 core 3.x

    //timerAlarmWrite(timer, 1000, true); //ESP32 core 2.x
    //timerAlarmEnable(timer); //ESP32 core 2.x

    // Set alarm to trigger every 1000 ticks (1ms)
    // Params: (timer, alarm_value, autoreload, reload_count)
    // Setting reload_count to 0 means it repeats indefinitely.
    timerAlarm(timer, 1000, true, 0); //ESP32 core 3.x. 1000 means this activates 1000000/1000 = 1000 times per second

    // Setup pin modes
    pinMode(PIN_I2S_DATA, OUTPUT);
    pinMode(PIN_I2S_BCK,  OUTPUT);
    pinMode(PIN_I2S_WS,   OUTPUT);

    pinMode(PIN_X_LIMIT, INPUT);     // Pin 36/35 are input-only, no pullup
    pinMode(PIN_Y_LIMIT, INPUT);
    pinMode(PIN_Z_LIMIT, INPUT);
    pinMode(PIN_Z_PROBE, INPUT);
    //pinMode(PIN_SD_DET,  INPUT);

    //pinMode(PIN_EXP2_TRB, INPUT);
    //pinMode(PIN_EXP2_TRA, INPUT);
    //pinMode(PIN_EXP2_CONFIRM, INPUT);
    //pinMode(PIN_EXP1_BACK, INPUT);

    //Setup i2c
    Wire.begin(OLED_SDA, OLED_SCL);

    // Setup i2c DS3231 module
    rtcDS3231.begin();

    // Setup OLED Menu
    #if DISPLAY_OUTPUT == SSD1306
      oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS); //check config
      oled.setFont(menuFont);
      oled.clear();
      oled.setCursor(0, 0);
    #elif DISPLAY_OUTPUT == SH1106
      oled.begin();
      oled.setFont(menuFont);
      oled.clear();
      oled.setCursor(0, 0);
    #endif

    #if DISPLAY_OUTPUT == SSD1306
      oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS); //check config
      oled.setFont(menuFont);
    #elif DISPLAY_OUTPUT == SH1106
    #endif
    oled.print("AltAzTracker Booting...");
    delay(2000);

    nav.showTitle = true; // Show titles in the menus and submenus
    nav.timeOut = 60;
    nav.idleTask = idle; //point a function to be used when menu is suspended
    nav.idleOn(); // Start with the main screen and not the menu

    // Flashing LED test
    pinMode(LED_PIN, OUTPUT);
    delay(250);
    pinMode(LED_PIN, INPUT);
    delay(250);

    // Setup stepper motors
    stepperAlt.setMaxSpeed(2500);
    stepperAlt.setAcceleration(1000);
    stepperAz.setMaxSpeed(2500);
    stepperAz.setAcceleration(1000);

    // Optional, set current date and time to compile time. This does not set UTC time, however...
    //rtcDS3231.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // Set ESP32Time from I2C RTC module
    DateTime now = rtcDS3231.now();
    rtcESP32Time.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());

    // Load preferences from "eeprom" (flash memory on ESP32) into global variables
    LoadPreferencesIntoMemory();

    //Update latitude and longitude hours, minutes and seconds variables from their decimal (double) in memory
    structDegreesMinutesSeconds latitudeDMS = decimalDegreesToDMS(latitudeDecimal);
    latitudeDMSDegrees = latitudeDMS.degrees;
    latitudeDMSMinutes = latitudeDMS.minutes;
    latitudeDMSSeconds = latitudeDMS.seconds; //Todo. Check decimalDegreesToDMS function as it appears to be able to produce seconds = 60.0

    structDegreesMinutesSeconds longitudeDMS = decimalDegreesToDMS(longitudeDecimal);
    longitudeDMSDegrees = longitudeDMS.degrees;
    longitudeDMSMinutes = longitudeDMS.minutes;
    longitudeDMSSeconds = longitudeDMS.seconds; //Todo. Check decimalDegreesToDMS function as it appears to be able to produce seconds = 60.0

    // Final tasks
    srWriteBitbang(0); // Clear shift register
    steppersEnable(false); // Disable steppers
    OLED_print("Setup done", INFO);
    delay(1000);
}



// #############
// # Main loop #
// #############
void loop() {
  // Poll the menu for the ArduinoMenu library
  nav.poll();

  //Receive and parse serial data
  serialRecieveLX200();
  if (serialNewData) {
    serialParseData();
  }

  // Move stepper motors if we are homed, a move has been requested and it is safe to do so
  if (hasHomePositionAlt && moveSteppersToTargetStepsAlt) { //We could also check for (stepperAlt.distanceToGo() != 0 || stepperAz.distanceToGo() != 0) but apparently this is handled automatically by accelstepper
    stepperAlt.moveTo(stepperAltTargetSteps);
    if (digitalRead(PIN_X_LIMIT) == HIGH && digitalRead(PIN_Y_LIMIT) == HIGH) {  
      stepperAlt.run();
    } else {
      //steppersDisableAndAbortAllCode();
      steppersEnable(false);
      steppersHaltMovement();
    }
  }

  // Move stepper motors if we are homed, a move has been requested and it is safe to do so
  if (hasHomePositionAz && moveSteppersToTargetStepsAz) { //We could also check for (stepperAlt.distanceToGo() != 0 || stepperAz.distanceToGo() != 0) but apparently this is handled automatically by accelstepper
    stepperAz.moveTo(stepperAzTargetSteps);
    if (digitalRead(PIN_Z_LIMIT) == HIGH && digitalRead(PIN_Z_PROBE) == HIGH) {  
      stepperAz.run();
    } else {
      //steppersDisableAndAbortAllCode();
      steppersEnable(false);
      steppersHaltMovement();
    }
  }

  //Every updateTrackingRADecInterval milliseconds, check if we are tracking an RA Dec target and if so, recalculate where we should be pointing in Alt Az based on the last given LX200TargetRA and LX200TargetDec
  updateTrackingRADecCurrentMillis = millis();
  if (updateTrackingRADecCurrentMillis - updateTrackingRADecPreviousMillis >= updateTrackingRADecInterval) {
    updateTrackingRADecPreviousMillis = updateTrackingRADecCurrentMillis;
    if (trackingRADecTarget) {
      positionSetTargetRADec(LX200TargetRA, LX200TargetDec);
    }
  }
}
