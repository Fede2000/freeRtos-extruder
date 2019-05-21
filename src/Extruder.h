#ifndef EXTRUDER_H
#define EXTRUDER_H

#include <Arduino_FreeRTOS.h>
#include "Thread.h"

class Extruder 
{
    public:
        float speed_rpm; //rpm target
        int timer;
        int steps = 0;
        bool is_step = false;
        bool was_step = false;
        bool is_enabled = true;
        bool retraction_is_enabled = true, should_step_retraction = false;

        Extruder();
        int setSpeedRpm(float speed);
        void incrementSpeed(int i);
        void runSpeed();
        bool retract(int nSteps);
        bool overExtrude(int nSteps);

        
    private:
        void Main();
        void setTimer(float period_us);
        
        float target_period_ms, boost = 1;
        float period_ms;
        float PERIOD_COSTANT_MS ;
        
};


#endif