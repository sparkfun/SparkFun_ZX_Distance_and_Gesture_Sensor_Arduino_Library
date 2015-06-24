/****************************************************************
UART_Gesture_Demo.ino
XYZ Interactive ZX Sensor
Shawn Hymel @ SparkFun Electronics
May 6, 2015
https://github.com/sparkfun/SparkFun_ZX_Distance_and_Gesture_Sensor_Arduino_Library

IMPORTANT: This feature is unsupported in the ZX sensor! This example only
kind of works. You will see gestures appear, but they will generally be about
1 gesture behind. A future update of the ZX's firmware is required to enable
this feature.

Tests the ZX sensor's ability to read gesture data over UART using 
SoftwareSerial. The ZX_Sensor library is needed for the
UART constants. As a result, we have to link the Wire library,
even though it is not used.

Hardware Connections:
 
 Arduino Pin  ZX Sensor Board  Function
 ---------------------------------------
 5V           VCC              Power
 GND          GND              Ground
 10           TX               Data

Resources:
Include SoftwareSerial.h, Wire.h, and ZX_Constants.h

Development environment specifics:
Written in Arduino 1.6.3
Tested with a SparkFun RedBoard

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include <SoftwareSerial.h>
#include <Wire.h>
#include <ZX_Sensor.h>

// Global Variables
SoftwareSerial soft_serial(10, 11);  // RX, TX
uint8_t c;

void setup() {
  
  // Initialize hardware serial port. Note the baud rate.
  Serial.begin(115200);
  Serial.println();
  Serial.println("-----------------------------------------");
  Serial.println("SparkFun/GestureSense - UART Gesture Demo");
  Serial.println("Note: higher 'speed' numbers mean slower");
  Serial.println("-----------------------------------------");
  
  // Initialize software serial port
  soft_serial.begin(115200);
}

void loop() {
  
  // Read in a character from the UART
  if ( soft_serial.available() ) {
    c = soft_serial.read();
    
    // Determine type of message and print it
    switch ( c ) {
      case ZX_UART_GESTURE:
      
        // Determine type of gesture
        c = soft_serial.read();
        switch ( c ) {
          case RIGHT_SWIPE:
            c = soft_serial.read();
            Serial.print("Right Swipe. Speed: ");
            Serial.println(c, DEC);
            break;
          case LEFT_SWIPE:
            c = soft_serial.read();
            Serial.print("Left Swipe. Speed: ");
            Serial.println(c, DEC);
            break;
          case UP_SWIPE:
            c = soft_serial.read();
            Serial.print("Up Swipe. Speed: ");
            Serial.println(c, DEC);
          default:
            break;
        }
        
        break;
      case ZX_UART_ID:
        Serial.print("Sensor type: ");
        c = soft_serial.read();
        Serial.print(c, DEC);
        Serial.print(" HW version: ");
        c = soft_serial.read();
        Serial.print(c, DEC);
        Serial.print(" FW version: ");
        c = soft_serial.read();
        Serial.println(c, DEC);
        break;
      default:
        break;
    }
  }
}