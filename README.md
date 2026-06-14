> [!WARNING]
> This project is currently work in progress. Not everything will work as intended

# AltAzMount
Alt Az (Pan Tilt) mount that tracks celestial objects based on Right Ascension and Declination. Can accept LX200 commands from Stellarium via serial

## 3D prints - SketchUp
Contains the main model for 3d printing. Uses SketchUp version 8.0.14346

## 3D prints - OpenSCAD
Contains the worm gear and spur gear for the mechanism. Uses gears.scad from https://github.com/chrisspen/gears

## ESP32 Code
Contains the code for running on the ESP32 controller. Uses an MKS DLC32 as the main control board.

## Python Code
Scripts that can control the mount to create an image based on the data received, or simulate visualisations of Alt Az movement.
