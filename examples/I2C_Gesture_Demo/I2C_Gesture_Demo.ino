/****************************************************************
I2C_Gesture_Demo.ino
XYZ Interactive ZX Sensor
Shawn Hymel @ SparkFun Electronics
July 30, 2014
https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

Tests the ZX sensor's ability to read ZX data over I2C. This demo
configures the ZX sensor and periodically polls for gestures.

Hardware Connections:
 
 Arduino Pin  ZX Sensor Board  Function
 ---------------------------------------
 5V           VCC              Power
 GND          GND              Ground
 A4           DA               I2C Data
 A5           CL               I2C Clock

Resources:
Include Wire.h and SFE_ZX_Sensor.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with an Arduino UNO R3

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include <Wire.h>
#include <SFE_ZX_Sensor.h>

// Constants
#define ZX_ADDR        0x10    // ZX Sensor I2C address

// Global Variables
SFE_ZX_Sensor zx_sensor = SFE_ZX_Sensor(ZX_ADDR);
GestureType gesture;

void setup() {

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println("SparkFun/GestureSense - I2C Gesture Demo");
  Serial.println("----------------------------------------");
  
  // Initialize ZX Sensor (configure I2C and read model ID)
  if ( zx_sensor.init() ) {
    Serial.println("ZX Sensor initialization complete");
  } else {
    Serial.println("Something went wrong during ZX Sensor init!");
  }
}

void loop() {
  
  // If there is gesture data available, read and print it
  if ( zx_sensor.gestureAvailable() ) {
    gesture = zx_sensor.readGesture();
    switch ( gesture ) {
      case RIGHT_SWIPE:
        Serial.println("Right Swipe");
        break;
      case LEFT_SWIPE:
        Serial.println("Left Swipe");
        break;
      case UP_SWIPE:
        Serial.println("Up Swipe");
        break;
      case HOVER:
        Serial.println("Hover");
        break;
      case HOVER_LEFT:
        Serial.println("Hover-Left");
        break;
      case HOVER_RIGHT:
        Serial.println("Hover-Right");
        break;
      case HOVER_UP:
        Serial.println("Hover-Up");
        break;
      default:
        break;
    }
  }
}