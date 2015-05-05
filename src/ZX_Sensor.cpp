/**
 * @file    SFE_ZX_Sensor.cpp
 * @brief   Library for the SparkFun/GestureSense ZX Sensor
 * @author  Shawn Hymel (SparkFun Electronics)
 *
 * @copyright	This code is public domain but you buy me a beer if you use
 * this and we meet someday (Beerware license).
 *
 * This library interfaces the XYZ Interactive ZX Sensor to Arduino over I2C.
 * The library relies on the Arduino Wire (I2C) library. To use the library,
 * instantiate a ZX_Sensor object, call init(), and call the desired
 * functions.
 */
 
#include <Arduino.h>
#include <Wire.h>

#include "SFE_ZX_Sensor.h"

/*******************************************************************************
 * Initialization
 ******************************************************************************/

/**
 * @brief Constructor - Instantiates SFE_ZX_Sensor object
 */
SFE_ZX_Sensor::SFE_ZX_Sensor(int address)
{
    addr_ = address;
}

/**
 * @brief Destructor
 */
SFE_ZX_Sensor::~SFE_ZX_Sensor()
{

}

/**
 * @brief Configures I2C communications and checks ZX sensor model number.
 *
 * @param enable_interrupts enables DR pin to assert on events
 * @return True if initialized successfully. False otherwise.
 */
bool SFE_ZX_Sensor::init(   InterruptType interrupts /* = NO_INTERRUPTS */,
                            bool active_high /* = true */)
{
    uint8_t ver;
    
    /* Initialize I2C */
    Wire.begin();
    
    /***TEST***/
    uint8_t val;
    if ( !wireReadDataByte(ZX_DRCFG, val) ) {
        return false;
    }
    Serial.print("DRCFG: 0x");
    Serial.println(val, HEX);
    
    /* Read Model ID and check against known values for the ZX Sensor */
    if ( !wireReadDataByte(ZX_MODEL, ver) ) {
        return false;
    }
    if ( ver != ZX_MODEL_VER ) {
        return false;
    }
    
    /* Read Register Map version and check against known good values */
    if ( !wireReadDataByte(ZX_REGVER, ver) ) {
        return false;
    }
    if ( ver != ZX_REG_MAP_VER ) {
        return false;
    }
    
    /* Enable DR interrupts based on desired interrupts */
    setInterruptTrigger(interrupts);
    configureInterrupts(active_high, false);
    if ( interrupts == NO_INTERRUPTS ) {
        disableInterrupts();
    } else {
        enableInterrupts();
    }
    
    /***TEST***/
    uint8_t val2;
    if ( !wireReadDataByte(ZX_DRCFG, val2) ) {
        return false;
    }
    Serial.print("DRCFG: 0x");
    Serial.println(val, HEX);
    
    return true;
}

/*******************************************************************************
 * Interrupt Configuration
 ******************************************************************************/

 /**
  * @brief Sets the triggers that cause the DR pin to change
  *
  * @param[in] interrupts which types of interrupts to enable
  * @return True if operation successful. False otherwise.
  */
bool SFE_ZX_Sensor::setInterruptTrigger(InterruptType interrupts)
{
    switch ( interrupts ) {
        case POSITION_INTERRUPTS:
            if ( !setRegisterBit(ZX_DRE, DRE_CRD) ) {
                return false;
            }
            break;
        case GESTURE_INTERRUPTS:
            if ( !setRegisterBit(ZX_DRE, DRE_SWP) ) {
                return false;
            }
            if ( !setRegisterBit(ZX_DRE, DRE_HOVER) ) {
                return false;
            }
            if ( !setRegisterBit(ZX_DRE, DRE_HVG) ) {
                return false;
            }
            break;
        case ALL_INTERRUPTS:
            if ( !wireWriteDataByte(ZX_DRE, SET_ALL_DRE) ) {
                return false;
            }
            break;
        default:
            if ( !wireWriteDataByte(ZX_DRE, 0x00) ) {
                return false;
            }
            break;
    }
    
    return true;
}

/**
 * @brief Configured the behavior of the DR pin on an interrupt
 *
 * @param[in] active_high true for DR active-high. False for active-low.
 * @param[in] pin_pulse true: DR pulse. False: DR pin asserts until STATUS read
 * @return True if operation successful. False otherwise.
 */
bool SFE_ZX_Sensor::configureInterrupts(    bool active_high, 
                                            bool pin_pulse /* = false */)
{
    /* Set or clear polarity bit to make DR active-high or active-low */
    if ( active_high ) {
        if ( !setRegisterBit(ZX_DRCFG, DRCFG_POLARITY) ) {
            return false;
        }
    } else {
        if ( !clearRegisterBit(ZX_DRCFG, DRCFG_POLARITY) ) {
            return false;
        }
    }
    
    /* Set or clear edge bit to make DR pulse or remain set until STATUS read */
    if ( pin_pulse ) {
        if ( !setRegisterBit(ZX_DRCFG, DRCFG_EDGE) ) {
            return false;
        }
    } else {
        if ( !clearRegisterBit(ZX_DRCFG, DRCFG_EDGE) ) {
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Turns on interrupts so that DR asserts on desired events.
 *
 * @return True if operation successful. False otherwise.
 */
bool SFE_ZX_Sensor::enableInterrupts()
{
    if ( !setRegisterBit(ZX_DRCFG, DRCFG_EN) ) {
        return false;
    }
    
    return true;
}

/**
 * @brief Turns off interrupts so that DR will never assert.
 *
 * @return True if operation successful. False otherwise.
 */
bool SFE_ZX_Sensor::disableInterrupts()
{
    if ( !clearRegisterBit(ZX_DRCFG, DRCFG_EN) ) {
        return false;
    }
    
    return true;
}

/**
 * @brief Reads the STATUS register to clear an interrupt (de-assert DR)
 *
 * @return True if operation successful. False otherwise.
 */
bool SFE_ZX_Sensor::clearInterrupt()
{
    uint8_t val;
    
    if ( !wireReadDataByte(ZX_STATUS, val) ) {
        return false;
    }
    
    return true;
}

/*******************************************************************************
 * Data available
 ******************************************************************************/

 /**
  * @brief Indicates that new position (X or Z) data is available
  *
  * @return True if data is ready to be read. False otherwise.
  */
bool SFE_ZX_Sensor::positionAvailable()
{
    uint8_t status;
    
    /* Read STATUS register and extract DAV bit */
    if ( !wireReadDataByte(ZX_STATUS, status) ) {
        return false;
    }
    status &= 0b00000001;
    if ( status ) {
        return true;
    }
    
    return false;
}

/**
 * @brief Indicates that a gesture is available to be read
 *
 * @return True if gesture is ready to be read. False otherwise.
 */
bool SFE_ZX_Sensor::gestureAvailable()
{
    uint8_t status;
    
    /* Read STATUS register and extract SWP bit */
    if ( !wireReadDataByte(ZX_STATUS, status) ) {
        return false;
    }
    status &= 0b00011100;
    if ( status ) {
        return true;
    }
    
    return false;
}

/*******************************************************************************
 * Sensor data read
 ******************************************************************************/

/**
 * @brief Reads the X position data from the sensor
 *
 * @return 0-240 for X position. 0xFF on read error.
 */
uint8_t SFE_ZX_Sensor::readX()
{
    uint8_t x_pos;
    
    /* Read X Position register and return it */
    if ( !wireReadDataByte(ZX_XPOS, x_pos) ) {
        return ZX_ERROR;
    }
    if ( x_pos > MAX_X ) {
        return ZX_ERROR;
    }
    return x_pos;
}

/**
 * @brief Reads the Z position data from the sensor
 *
 * @return 0-240 for Z position. 0xFF on read error.
 */
uint8_t SFE_ZX_Sensor::readZ()
{
    uint8_t z_pos;
    
    /* Read X Position register and return it */
    if ( !wireReadDataByte(ZX_ZPOS, z_pos) ) {
        return ZX_ERROR;
    }
    if ( z_pos > MAX_Z ) {
        return ZX_ERROR;
    }
    return z_pos;
}

/**
 * @brief Reads the last detected gesture from the sensor
 *
 * 0x01 Right Swipe
 * 0x02 Left Swipe
 * 0x03 Up Swipe
 * 0x05 Hover
 * 0x06 Hover-Left
 * 0x07 Hover-Right
 * 0x08 Hover-Up
 *
 * @return a number corresponding to  a gesture. 0xFF on error.
 */
GestureType SFE_ZX_Sensor::readGesture()
{
    uint8_t gesture;
    
    /* Read GESTURE register and return the value */
    if ( !wireReadDataByte(ZX_GESTURE, gesture) ) {
        return NO_GESTURE;
    }
    switch ( gesture ) {
        case RIGHT_SWIPE:
            return RIGHT_SWIPE;
        case LEFT_SWIPE:
            return LEFT_SWIPE;
        case UP_SWIPE:
            return UP_SWIPE;
        case HOVER:
            return HOVER;
        case HOVER_LEFT:
            return HOVER_LEFT;
        case HOVER_RIGHT:
            return HOVER_RIGHT;
        case HOVER_UP:
            return HOVER_UP;
        default:
            return NO_GESTURE;
    }
}

/*******************************************************************************
 * Bit Manipulation
 ******************************************************************************/
 
/**
 * @brief sets a bit in a register over I2C
 *
 * @param[in] bit the number of the bit (0-7) to set
 * @return True if successful write operation. False otherwise.
 */
bool SFE_ZX_Sensor::setRegisterBit(uint8_t reg, uint8_t bit)
{
    uint8_t val;
    
    /* Read value from register */
    if ( !wireReadDataByte(reg, val) ) {
        return false;
    }
    
    /* Set bits in register and write back to the register */
    val |= (1 << bit);
    if ( !wireWriteDataByte(reg, val) ) {
        return false;
    }
    
    return true;    
}

/**
 * @brief clears a bit in a register over I2C
 *
 * @param[in] bit the number of the bit (0-7) to clear
 * @return True if successful write operation. False otherwise.
 */
bool SFE_ZX_Sensor::clearRegisterBit(uint8_t reg, uint8_t bit)
{
    uint8_t val;
    
    /* Read value from register */
    if ( !wireReadDataByte(reg, val) ) {
        return false;
    }
    
    /* Clear bit in register and write back to the register */
    val &= ~(1 << bit);
    if ( !wireWriteDataByte(reg, val) ) {
        return false;
    }
    
    return true;    
} 

/*******************************************************************************
 * Raw I2C Reads and Writes
 ******************************************************************************/

/**
 * @brief Writes a single byte to the I2C device (no register)
 *
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
bool SFE_ZX_Sensor::wireWriteByte(uint8_t val)
{
    Wire.beginTransmission(addr_);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }
    
    return true;
}

/**
 * @brief Writes a single byte to the I2C device and specified register
 *
 * @param[in] reg the register in the I2C device to write to
 * @param[in] val the 1-byte value to write to the I2C device
 * @return True if successful write operation. False otherwise.
 */
bool SFE_ZX_Sensor::wireWriteDataByte(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(addr_);
    Wire.write(reg);
    Wire.write(val);
    if( Wire.endTransmission() != 0 ) {
        return false;
    }

    return true;
}

/**
 * @brief Reads a single byte from the I2C device and specified register
 *
 * @param[in] reg the register to read from
 * @param[out] the value returned from the register
 * @return True if successful read operation. False otherwise.
 */
bool SFE_ZX_Sensor::wireReadDataByte(uint8_t reg, uint8_t &val)
{
    
    /* Indicate which register we want to read from */
    if (!wireWriteByte(reg)) {
        return false;
    }
    
    /* Read from register */
    Wire.requestFrom(addr_, 1);
    while (Wire.available()) {
        val = Wire.read();
    }

    return true;
}