#include "Extruder.h"
#include <Arduino_FreeRTOS.h>
#include "configuration.h"


Extruder::Extruder(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{

}

void Extruder::Main() {
    for (;;)
    {
        if(is_step){  //abs(period_ms - target_period_ms) > 0.0001
            period_ms = period_ms * 0.6 + target_period_ms * 0.4;
            setTimer( period_ms );
            Serial.println(period_ms - target_period_ms);
        }
        else
            period_ms = 0;
       vTaskDelay(ticks);
    }
}


// set timer 4-A
void Extruder::setTimer(float period_ms){
    timer = int(period_ms * (2000)); 
    //OCR3B = timer;    // period * (freq / prescaler )  
    //Serial.println(timer);
}

void Extruder::setSpeed(float speed_rpm){
    // (speed steps/second) : speed_rpm / 60 *200*32*3.75  -> period = 1/(speed steps/second)
    target_period_ms = 2.5 /speed_rpm;
    //setTimer( period_ms );
    
}
void Extruder::setSpeed(){
    target_period_ms = 2.5 /speed_rpm;
    //setTimer( period_ms );
}


AccelStepper extruder1(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
//int16_t ESet = 30;   // rpm