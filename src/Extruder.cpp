#include "Extruder.h"
//#include <Arduino_FreeRTOS.h>
#include "configuration.h"

/*
Extruder::Extruder(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{

}

void Extruder::Main() {
    for (;;)
    {
        if(is_step){  //abs(period_ms - target_period_ms) > 0.0001
            period_ms = period_ms * 0.2 + target_period_ms * 0.8;
            setTimer( period_ms );
            Serial.println(period_ms - target_period_ms);
        }
        else
            period_ms = 2.5;
       vTaskDelay(ticks);
    }
}
*/

Extruder::Extruder(){
    setSpeedRpm(0);
}

// set timer 4-A
void Extruder::setTimer(float target_period_ms){
    if(is_step){
        /*
        if((period_ms - target_period_ms) > 0.00001 )
            period_ms -= 0.001;
        else if ((period_ms - target_period_ms) < -0.00001)
            period_ms += 0.001;
        else 
            period_ms = target_period_ms;
        */
        period_ms = period_ms * 0.8 + target_period_ms * 0.2;
    }
    else
        period_ms = 1;

    timer = int(period_ms * (2000)); 
}

void Extruder::runSpeed(){
    target_period_ms = 2.5 /speed_rpm;
    setTimer( target_period_ms );
}

void Extruder::setSpeedRpm(float speed){
    speed_rpm = speed;
    if(speed > MAX_SET_TEMP)
        speed_rpm = MAX_SET_TEMP;
    else if(speed <= 0){
        speed_rpm = 0;
        is_step = false;
    }
}

void Extruder::incrementSpeed(int i){
    setSpeedRpm(speed_rpm + i);
}


AccelStepper extruder1(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
//int16_t ESet = 30;   // rpm