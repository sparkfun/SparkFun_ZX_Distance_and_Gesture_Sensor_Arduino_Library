/****************************************************************
I2C_ZX_Interrupt.ino
XYZ Interactive ZX Sensor
Shawn Hymel @ SparkFun Electronics
July 31, 2014
https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

Tests the ZX sensor's ability to read ZX data over I2C using an
interrupt pin. This program configures I2C and sets up an
interrupt to occur whenever the ZX sensor throws its DR pin high.

Hardware Connections:
 
 Arduino Pin  ZX Sensor Board  Function
 ---------------------------------------
 5V           VCC              Power
 GND          GND              Ground
 A4           DA               I2C Data
 A5           CL               I2C Clock
 2            DR               Data Ready

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
#define INTERRUPT_NUM  0       // Pin 2 on the UNO

// Global Variables
SFE_ZX_Sensor zx_sensor = SFE_ZX_Sensor(ZX_ADDR);
uint8_t x_pos;
uint8_t z_pos;

void setup() {

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("----------------------------------------");
  Serial.println("SparkFun/GestureSense - I2C ZX Interrupt");
  Serial.println("----------------------------------------");
  
  // Initialize interrupt service routine
  attachInterrupt(INTERRUPT_NUM, interruptRoutine, RISING);
  
  // Initialize ZX Sensor (configure I2C and read model ID)
  if ( zx_sensor.init(GESTURE_INTERRUPTS) ) {
    Serial.println("ZX Sensor initialization complete");
  } else {
    Serial.println("Something went wrong during ZX Sensor init!");
  }
}

void loop() {
  
  // Do nothing
  delay(1);
}

void interruptRoutine() {
  
  // You MUST read the STATUS register to clear interrupt!
  zx_sensor.clearInterrupt();
  
  // Read last X and Z coordinates
  x_pos = zx_sensor.readX();
  if ( x_pos != ZX_ERROR ) {
    Serial.print("X: ");
    Serial.print(x_pos);
  }
  z_pos = zx_sensor.readZ();
  if ( z_pos != ZX_ERROR ) {
    Serial.print(" Z: ");
    Serial.println(z_pos);
  }
}