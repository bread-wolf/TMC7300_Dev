/*
 * TMC7300_Lib.cpp
 *
 *  Created on: 13.07.2021
 *      Author: AB
 */

#include "TMC7300_Lib.hpp"

#define TMC7300_VERSION_BYTE    0x40  /* Chip version */
#define TMC7300_V_FULLSCALE     325   /* Full scale voltage for torque limiter */


TMC7300::TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin)
    : TMCSerial(serialPort, baudrate, chipAddress), _enablePin(enablePin), _isConfigured(false)
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
    writeField(TMC7300_ENABLEDRV, false);

    /* Write fundamental parameters. */
    TMCSerial::writeField(TMC7300_PWM_DIRECT, true);
    TMCSerial::writeField(TMC7300_TEST_MODE, false);

    return true;
}

uint32_t TMC7300::configDriver(bool useExtcap, bool useParallel, uint32_t senseResistor, uint32_t currentLim)
{
    uint8_t currentDivider;

    /* Limit some input values. */
    if ((currentLim > 2400) && (useParallel))
    {
        /* In parallel mode, current can go up to 2.4 Amps */
        currentLim = 2400;
    }
    if ((currentLim > 1400) && (!useParallel))
    {
        /* Current limit is lower when using 2 motors. */
        currentLim = 1400;
    }

    if (senseResistor < 80)
    {
        /* Sense resistor value should not go under 100mOhm */
        senseResistor = 80;
    }

    /* Save values in class attribute. */
    _useExtcap = useExtcap;
    _useParallel = useParallel;

    /* Calculate current scaling, and set default current limit to */
    currentDivider = (currentLim * 32 * (senseResistor + 30) / TMC7300_V_FULLSCALE) - 1;
    _currentLim = ((currentDivider + 1) * TMC7300_V_FULLSCALE) / (32 * (_senseResistor + 30));
    _senseResistor = senseResistor;
    
    /* Set parameters */
    TMCSerial::writeField(TMC7300_EXTCAP, useExtcap);
    TMCSerial::writeField(TMC7300_PAR_MODE, useParallel);
    TMCSerial::writeField(TMC7300_IRUN, currentDivider);

    /* Return and store actual current limit and set config flag to true. */
    _isConfigured = true;
    return _currentLim;
}

void TMC7300::enableDriver(bool enable)
{
    if (!_isConfigured)
    {
        /* If the driver wasn't configured, it is not safe to enable it. */
        return;
    }

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