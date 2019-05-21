#include "Extruder.h"
#include "configuration.h"
#include "Arduino.h"
/*
formula: 
    periodo [seconds] = 3/10 * 1/ (RPM * MICROSTEPPINGS * GEAR_REDUCTION)
    period = 3/(10*MICROSTEPPINGS*GEAR_REDUCTION) [costant] * 1/RPM
    period

    retract di 5 step->
*/

Extruder::Extruder(){
    setSpeedRpm(0);
    PERIOD_COSTANT_MS = 3.0 /(10.0 *(float)MICROSTEPPINGS*(float)GEAR_REDUCTION) * 1000.0;
}

// set timer 4-A
void Extruder::setTimer(float target_period_ms){
    if(is_step || should_step_retraction){
        period_ms = period_ms * 0.8 + target_period_ms * 0.2;
    }
    //else
    //    period_ms = 2.5;
    // tick period = 4*10^(-6) [s]
    // timer = int(period_ms /tick period); -->
    // timer = int(period_ms *250); 

    // number of ticks
    timer = int(period_ms * 250);
}

void Extruder::runSpeed(){
    //target_period_ms = 2.5 /speed_rpm;
    target_period_ms = PERIOD_COSTANT_MS / speed_rpm; //* boost;
    setTimer( target_period_ms );
}
//  nSteps always > 0
bool Extruder::retract(int nSteps){
    if(retraction_is_enabled){
        period_ms = 2.5;
        PORTA |= 1 << PORTA6;
        steps -= nSteps;
        //boost = 1;
        should_step_retraction = true;
    }
}

bool Extruder::overExtrude(int nSteps){
    period_ms = 2.5;
    PORTA &= ~(1 << PORTA6);
    if(retraction_is_enabled){
        //steps -= nSteps;
        //boost = 0.5;
        should_step_retraction = false;
    }
}

int Extruder::setSpeedRpm(float speed){
    
    if(speed > MAX_SET_SPEED){
        speed_rpm = MAX_SET_SPEED;
        return 0;
    }
    else if(speed <= 0){
        speed_rpm = 0;
        is_step = false;
        digitalWrite(E_ENABLE_PIN,HIGH); // free the motor
        return 0;
    }
    digitalWrite(E_ENABLE_PIN,LOW); //TODO: evitare digitalWrite ogni volta
    speed_rpm = speed;
}

void Extruder::incrementSpeed(int i){
    setSpeedRpm(speed_rpm + i);
}

