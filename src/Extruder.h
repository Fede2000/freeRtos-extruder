#ifndef EXTRUDER_H
#define EXTRUDER_H

#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include "Thread.h"

class Extruder : public Thread
{
    public:
        float speed_rpm; //rpm target
        int timer;
        bool is_step = false;
        
        Extruder( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks );
        
        
        void setSpeed(float speed_rpm);
        void setSpeed();
        int getSpeed(); // not yet implemented
        
    private:
        void Main();
        void setTimer(float period_us);
        
        float target_period_ms;
        float period_ms;
        uint32_t ticks;
};


extern AccelStepper extruder1;
//extern int ESet;

#endif