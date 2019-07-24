#ifndef EXTRUDER_H
#define EXTRUDER_H

#include <Arduino_FreeRTOS.h>
#include "Thread.h"

class Extruder 
{
    public:
        float speed_rpm; //rpm target
        int timer = 10000;
        int steps = 0;  //counter
        int steps_to_retract = 500;

        bool is_input_step = false;
        bool last_input_step = false;
        bool is_enabled = true;
        bool is_retraction_enabled = true, run_retraction = false;

        Extruder();
        int setSpeedRpm(float speed);
        void incrementSpeed(int i);
        void incrementRetraction(int i);
        void setStatus(bool status);
        void main();
        bool retract();
        bool overExtrude();

        
    private:
        void Main();
        void setTimer(float period_us);
        
        float target_period_ms, boost_period = 1; // boost_period = period multiplier -> used to increase velocity in order to recover lost steps
        float period_ms;
        float PERIOD_COSTANT_MS ;
        int prescaler = 64;
        int timer_frequency_mHz = 16000 / prescaler;
        
};


#endif