/****************************************************************
UART_ZX_Demo.ino
XYZ Interactive ZX Sensor
Shawn Hymel @ SparkFun Electronics
May 6, 2015
https://github.com/sparkfun/SparkFun_ZX_Distance_and_Gesture_Sensor_Arduino_Library

Tests the ZX sensor's ability to read ZX data over UART using 
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
  Serial.println("------------------------------------");
  Serial.println("SparkFun/GestureSense - UART ZX Demo");
  Serial.println("------------------------------------");
  
  // Initialize software serial port
  soft_serial.begin(115200);
}

void loop() {
  
  // Read in a character from the UART
  if ( soft_serial.available() ) {
    c = soft_serial.read();
    
    // Determine type of message and print it
    switch ( c ) {
      case ZX_UART_END:
        Serial.println("End of stream");
        break;
      case ZX_UART_RANGES:
        Serial.print("Ranges: ");
        c = soft_serial.read();
        Serial.print(c, DEC);
        Serial.print(", ");
        c = soft_serial.read();
        Serial.println(c, DEC);
        break;
      case ZX_UART_X:
        Serial.print("X: ");
        c = soft_serial.read();
        Serial.println(c, DEC);
        break;
      case ZX_UART_Z:
        Serial.print("Z: ");
        c = soft_serial.read();
        Serial.println(c, DEC);
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