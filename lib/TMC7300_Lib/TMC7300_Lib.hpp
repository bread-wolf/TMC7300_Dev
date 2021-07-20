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

enum class TMC7300_PwmFrequency
{
    FREQ_20K,   // 23.4kHz, Default, recommended value
    FREQ_35K,   // 31.5kHz, Out of hearing range
    FREQ_45K,   // 46.9kHz, For low inductance motors only
    FREQ_58K,   // 58.5kHz, For low inductance motors only
};

enum class TMC7300_FreewheelMode
{
    NO_FREEWHEEL,
    FREEWHEEL,
    SHORT_VIA_LS,
    SHORT_VIA_HS,
};

enum class TMC7300_BlankTime
{
    CLK16,  // 1.3us typ. (Recommended)
    CLK24,  // 2.0us typ. (Recommended)
    CLK32,  // 2.6us typ.
    CLK40,  // 3.3us typ.
};

class TMC7300 : private TMCSerial
{
    public:
        /* Creates instance, sets up default configuration. */
        TMC7300(HardwareSerial& serialPort, uint32_t baudrate, uint8_t chipAddress, uint8_t enablePin);

        /* Checks driver is present, put driver in a disabled safe state for configuration. */
        bool begin();

        /* Set up mandatory system parameters. Driver can't be enabled unless this function was called. 
         * Returns actual current limit value in mA.
         * 
         * senseResistor [mOhm]
         * currentLim    [mA] 
         */
        uint32_t configDriver(bool useExtcap, bool useParallel, uint32_t senseResistor, uint32_t currentLim);

        /* After creating object and calling begin(), driver is disabled and in safe state.
         * This function return false and does nothing is configDriver wasn't called first.
         *
         * This function acts on the selected enable pin, as well as internal enable bit. */
        void enableDriver(bool enable);

        /* Set desired freewheel/breaking mode, to activate freewheeling use the enable function. */
        void setFreewheelMode(TMC7300_FreewheelMode mode);
        void enableFreewheelMode(bool enable);

        /* Set motor PWM frequency, see enum for possible options. */
        void setPwmFreq(TMC7300_PwmFrequency freq);

        /* Set comparator blank time, to correctly measure current.
         * Ignore comparator after switching event for blankTime to filter out ringing. */
        void setBlankTime(TMC7300_BlankTime blankTime);

        /* Check chip is alive : 
         *    - If chip is unconfigured, check version number
         *    - If chip is configured, check error flag 
         */
        bool isChipAlive();

    private:
        /* Driver enable pin */
        uint8_t _enablePin;

        /* Driver state and statuses */
        bool _isConfigured;

        /* Basic chip configuration */
        bool _useExtcap, _useParallel;
        uint32_t _senseResistor, _currentLim;

        /* Chopper configuration parameters */
        uint8_t _freewheelMode, _pwmFreq, _blankTime;
        int16_t _pwmRatioA, _pwmRatioB;
};



#endif /* TMC7300_LIB_H_ */