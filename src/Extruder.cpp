#include "Extruder.h"
#include "configuration.h"

/*
formula: 
    periodo [seconds] = 3/10 * 1/ (RPM * MICROSTEPPINGS * GEAR_REDUCTION)
    period = 3/(10*MICROSTEPPINGS*GEAR_REDUCTION) [costant] * 1/RPM
    period 
*/

Extruder::Extruder(){
    setSpeedRpm(0);
    PERIOD_COSTANT_MS = 3.0 /(10.0 *(float)MICROSTEPPINGS*(float)GEAR_REDUCTION) * 1000.0;
    Serial.println("PERIOD_COSTANT_MS");
    Serial.println(PERIOD_COSTANT_MS);
}

// set timer 4-A
void Extruder::setTimer(float target_period_ms){
    if(is_step){
        period_ms = period_ms * 0.8 + target_period_ms * 0.2;
    }
    else
        period_ms = 1;
    // tick period = 4*10^(-6) [s]
    // timer = int(period_ms /tick period); -->
    // timer = int(period_ms *250); 

    // number of ticks
    timer = int(period_ms * 250);
}

void Extruder::runSpeed(){
    //target_period_ms = 2.5 /speed_rpm;
    target_period_ms = PERIOD_COSTANT_MS / speed_rpm;
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
