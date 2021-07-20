/*
 * TMC7300_Lib.cpp
 *
 *  Created on: 13.07.2021
 *      Author: AB
 */

#include "TMC7300_Lib.hpp"

#define TMC7300_VERSION_BYTE    0x40  /* Chip version. */
#define TMC7300_V_FULLSCALE     325   /* Full scale voltage for torque limiter. */
#define TMC7300_MAX_CURR_PAR    2000  /* Max motor current in parallel mode. */
#define TMC7300_MAX_CURR_STD    1200  /* Max motor current in two motor mode. */
#define TMC7300_MIN_RSENSE      80    /* Minimum recommended value for R_sense. */


TMC7300::TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin)
    : TMCSerial(serialPort, baudrate, chipAddress), _enablePin(enablePin), _isConfigured(false), _isFreeWheelEnabled(false)
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
    if ((currentLim > TMC7300_MAX_CURR_PAR) && (useParallel))
    {
        /* In parallel mode, current can go up to 2 Amps */
        currentLim = TMC7300_MAX_CURR_PAR;
    }
    if ((currentLim > TMC7300_MAX_CURR_STD) && (!useParallel))
    {
        /* Current limit is lower when using 2 motors. */
        currentLim = TMC7300_MAX_CURR_STD;
    }

    if (senseResistor < TMC7300_MIN_RSENSE)
    {
        /* Sense resistor value should not go under 100mOhm */
        senseResistor = TMC7300_MIN_RSENSE;
    }

    /* Save values in class attribute. */
    _useExtcap = useExtcap;
    _useParallel = useParallel;

    /* Calculate current scaling and actual current limit. */
    currentDivider = (currentLim * 32 * (senseResistor + 30) / TMC7300_V_FULLSCALE * 1000) - 1;
    if (currentDivider > 32)
        currentDivider = 32;

    _currentLim = ((currentDivider + 1) * TMC7300_V_FULLSCALE) * 1000 / (32 * (_senseResistor + 30));
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

void TMC7300::setFreewheelMode(TMC7300_FreewheelMode mode)
{
    TMCSerial::writeField(TMC7300_FREEWHEEL, mode);
    _freewheelMode = mode;
}

void TMC7300::enableFreewheelMode(bool enable)
{
    TMCSerial::writeField(TMC7300_MOTORRUN, !enable);
    _isFreeWheelEnabled = !enable;
}

void TMC7300::setPwmFreq(TMC7300_PwmFrequency freq)
{
    TMCSerial::writeField(TMC7300_TBL, freq);
    _pwmFreq = freq;
}

void TMC7300::setBlankTime(TMC7300_BlankTime blankTime)
{
    TMCSerial::writeField(TMC7300_TBL, blankTime);
    _blankTime = blankTime;
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