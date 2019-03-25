#include "Extruder.h"
#include <AccelStepper.h>
#include "configuration.h"

// set timer 4-A
void Extruder::setTimer(float period_ms){
    timer = int(period_ms * (2000)); 
    OCR3B = timer;    // period * (freq / prescaler )  
    Serial.println(timer);
}

void Extruder::setSpeed(float speed_rpm){
    // (speed steps/second) : speed_rpm / 60 *200*32*3.75  -> period = 1/(speed steps/second)
    float period_ms = 2.5 /speed_rpm;
    setTimer( period_ms );
}
void Extruder::setSpeed(){
    float period_ms = 2.5 /speed_rpm;
    setTimer( period_ms );
}


AccelStepper extruder1(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);
//int16_t ESet = 30;   // rpm