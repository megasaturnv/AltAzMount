> [!WARNING]
> This project is currently work in progress. Not everything will work as intended

# AltAzMount
Alt Az (Pan Tilt) mount that tracks celestial objects based on Right Ascension and Declination. Can accept LX200 commands from Stellarium via serial

## ESP32 Code
Contains the code for running on the ESP32 controller. Uses an MKS DLC32 as the main control board.

## Python Code
Scripts that can control the mount to create an image based on the data received, or simulate visualisations of Alt Az movement.

## 3D Print Design 1 - Original Design (Old)

This design is now deprecated and it is recommended to use design 2 in this repository. The worm gears gave good precision but the large size introduced a lot of flex in the design

M3 screws and nut of various sizes required to build this mount. 4x M4 screws and nuts required for VESA mount.

## 3D Print Design 2 - Remix of isaac879
### This is remixed from isaac879's design here:
<https://github.com/isaac879/Pan-Tilt-Mount>  
<https://www.thingiverse.com/thing:4547074>  

### List of changes:
* Added 18650s
* Added OLED + rotary encoder board, 18650 USB C charge controller, balance board and battery guage
* Added quick mount for tripods (varies slightly per tripod)
* Added stepper motor sun shields
* Added RTC module mount
* Added 40mm fan mount
* Added limit switch for altitude (tilt) axis
* Extended altitude (tilt) axis by 25mm upwards
* Modified camera mount to use VESA with 75mm spacing
* Modified design to use MKS DLC32 circuit board
* Square M3 nuts modified to take M3 hex nuts

M3 screws and nut of various sizes required to build this mount. 4x M4 screws and nuts required for VESA mount.

![image of Alt Az Mount design 2](/3D%20Print%20Design%202%20-%20Remix%20of%20isaac879/image.jpg)
