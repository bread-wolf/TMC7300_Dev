/*
 * TMC7300_Lib.hpp
 *
 *  Created on: 13.07.2021
 *      Author: AB
 */

#ifndef TMC7300_LIB_H_
#define TMC7300_LIB_H_

#include<Arduino.h>
#include<stdint.h>
#include<stdbool.h>

#include"TMCSerial.hpp"
#include"TMC7300/TMC7300_Map.hpp"

enum class TMC7300_Motor
{

}

class TMC7300
{
    public:
        /* Creates instance, sets up default configuration. */
        TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin);

        /* Checks driver is present, put driver in safe state for configuration. */
        bool begin();

        /* Set up mandatory system parameters. Driver can't be enabled unless this function was called. */
        bool configDriver(bool useExtcap, bool useParallel, uint32_t senseResistor);

        /* After creating object and calling begin(), driver is disabled and in safe state.
         * This function return false and does nothing is configDriver wasn't called first.
         *
         * This function acts on the selected enable pin, as well as internal enable bit. */
        bool enableDriver(bool enable);

        /* */
        void setPWM();
        void getPWM();

        /**/
        void setFreewheelMode();

        /**/
        void setPwmFreq();

        /**/
        void setBlankTime();

        /* Check chip is alive : 
         *    - If chip is unconfigured, check version number
         *    - If chip is configured, check error flag 
         */
        bool isChipAlive();

    private:
        /* Chip communication parameters */
        HardwareSerial& _serialPort;
        uint32_t _baudrate;
        uint8_t _chipAddress;
        uint8_t _enablePin;

        /* Driver state and statuses */
        bool _isConfigured;

        /* Basic chip configuration */
        bool _useExtcap, _useParallel;
        uint32_t _senseResistor;

        /* Chopper configuration parameters */
        uint8_t _freewheelMode, _pwmFreq, _blankTime;
        int16_t _pwmRatioA, _pwmRatioB;
};



#endif /* TMC7300_LIB_H_ */