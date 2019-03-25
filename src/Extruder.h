#ifndef EXTRUDER_H
#define EXTRUDER_H

#include <AccelStepper.h>

class Extruder 
{
    public:
        float speed_rpm; //rpm
        void setSpeed(float speed_rpm);
        void setSpeed();
        int getSpeed(); // not yet implemented
        int timer;
    private:
        void setTimer(float period_us);
};


extern AccelStepper extruder1;
//extern int ESet;

#endif