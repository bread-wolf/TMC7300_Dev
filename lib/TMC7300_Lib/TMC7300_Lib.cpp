/*
 * TMC7300_Lib.cpp
 *
 *  Created on: 13.07.2021
 *      Author: AB
 */

#include "TMC7300_Lib.hpp"
TMC7300::TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin)
    : TMCSerial(serialPort, baudrate, chipAddress), _enablePin(enablePin), _isConfigured(false), _isAlive(false)
{

}

bool TMC7300::begin()
{
    /* Call base class begin() to set up UART peripheral. */
    TMCSerial::begin();

    /* Check that chip is alive. */
    

    /* Set up enable pin. */
    pinMode(_enablePin, OUTPUT);

    /* Completely disable driver. */
    digitalWrite(_enablePin, LOW);
    TMCSerial::writeField(TMC7300_ENABLEDRV, false);

    /* Write default parameters. */
    TMCSerial::writeField();


}