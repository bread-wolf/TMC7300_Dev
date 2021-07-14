/*
 * TMC7300_Lib.cpp
 *
 *  Created on: 13.07.2021
 *      Author: AB
 */

#include "TMC7300_Lib.hpp"

#define TMC7300_VERSION_BYTE    0x40

TMC7300::TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin)
    : TMCSerial(serialPort, baudrate, chipAddress), _enablePin(enablePin), _isConfigured(false), _isAlive(false)
{

}

bool TMC7300::begin()
{
    /* Call base class begin() to set up UART peripheral. */
    TMCSerial::begin();

    /* Check that chip is alive. */
    if (isChipAlive() != true)
        return false;

    /* Set up enable pin. */
    pinMode(_enablePin, OUTPUT);

    /* Completely disable driver. */
    digitalWrite(_enablePin, LOW);
    TMCSerial::writeField(TMC7300_ENABLEDRV, false);

    /* Write fundamental parameters. */
    TMCSerial::writeField(TMC7300_PWM_DIRECT, true);
    TMCSerial::writeField(TMC7300_TEST_MODE, false);

    return true;
}


void TMC7300::enableDriver(bool enable)
{
    /* Enable or disable driver. */
    if (enable)
    {
        digitalWrite(_enablePin, HIGH);
    }
    else
    {
        digitalWrite(_enablePin, LOW);
    }
    
    TMCSerial::writeField(TMC7300_ENABLEDRV, enable);
}

bool TMC7300::isChipAlive()
{
    if (_isConfigured != true)
    {
        /* If driver has not been configured, first check that we can read registers. */
        TMCSerial::enableReadChecksum(true);
        int8_t serialErr;

        if ((TMCSerial::readField(TMC7300_VERSION, serialErr) != TMC7300_VERSION_BYTE) || serialErr < 0)
        {
            /* If we have the wrong chip, or a checksum error, fail here. */
            return false;
        }

        /* If checksum just passed, chances are it should be fine most of the time.
         * Disable here to save on computing. */
        TMCSerial::enableReadChecksum(false);        
    }

    /* If this passes, we check some more parameters. */
    if (TMCSerial::readField(TMC7300_DRV_ERR) == true)
    {
        /* Overtemperature or short circuit protection was tripped, yikes! */
        return false;
    }

    if (TMCSerial::readField(TMC7300_U3V5) == true)
    {
        /* Power supply voltage just went under 3.5V. Usually we want it to be at least 6V. */
        return false;
    }

    return true;    
}