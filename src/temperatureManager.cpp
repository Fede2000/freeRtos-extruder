#include "temperatureManager.h"
#include "configuration.h"
#include <PID_v1.h>
#include <Arduino.h>

TemperatureManager::TemperatureManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    myPID(&temperature, &output, &tempSetpoint, (double) CONST_KP, (double) CONST_KI, (double) CONST_KD, (int) 0 /*PID::DIRECT*/) , 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(0,MAX_PID_OUT);
    tempSetpoint = 35; // derault value
    alpha = 0.45;
    HEATER_ENABLED = false;
}

void TemperatureManager::getTemperature(){

    double average;
 
    //analogRead(THERMISTOR_PIN);                     // reading stability bugfix  https://electronics.stackexchange.com/questions/213851/arduino-analogread-neighbor-pin-noise-on-adc-even-with-big-delay
    average = (double) analogRead(THERMISTOR_PIN);  // reading stability bugfix

    average = 1023.0 / average - 1;
    average = (double) SERIESRESISTOR / average;
   
    // steinhart formula

    average = average / (double) THERMISTORNOMINAL;         // (R/Ro)
    average = log(average);                                 // ln(R/Ro)
    average /= B_COEFFICIENT;                               // 1/B * ln(R/Ro)
    average += 1.0 / (TEMPERATURENOMINAL + 273.15);         // + (1/To)
    average = 1.0 / average;                                // Invert
    average -= 273.15;                                      // convert to C
    temperature = average*alpha + temperature*(1-alpha);   //FIR filter

    //TCCR2B = TCCR2B & B11111000 | B00000111; 
    TCCR2B |=  (1<<CS41) | (1<<CS41) | (1<<CS40); // set pwm frequency (pin 10 & 9) to 30.64Hz: prescaler 1024

}
double TemperatureManager::readTemperature(){
    return temperature;
}

void TemperatureManager::Main() {
    #ifdef PREVENT_THERMAL_RUNAWAY        
        PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = true;
        bool THERMAL_RUNAWAY_TEMP_FLAG = false;
        unsigned long THERMAL_RUNAWAY_AT = millis();
    #endif //PREVENT_THERMAL_RUNAWAY

    #ifdef PREVENT_COLD_EXTRUSION
        COLD_EXTRUSION_FLAG = false; // TODO: true or false
        PREVENT_COLD_EXTRUSION_IS_ACTIVE = true;
    #endif //PREVENT_COLD_EXTRUSION

    for (;;)
    {
        getTemperature();
        myPID.Compute();

        #ifdef PREVENT_COLD_EXTRUSION
        if( PREVENT_COLD_EXTRUSION_IS_ACTIVE)
            COLD_EXTRUSION_FLAG = temperature > (tempSetpoint - PREVENT_COLD_EXTRUSION_DELTA_TEMP) ? false : true;
        else  
            COLD_EXTRUSION_FLAG = false;
        #endif
        
        #ifdef PREVENT_THERMAL_RUNAWAY
        if( PREVENT_THERMAL_RUNAWAY_IS_ACTIVE ){
            if( (millis() - THERMAL_RUNAWAY_AT) > THERMAL_RUNAWAY_PERIOD && HEATER_ENABLED){
                if(abs(tempSetpoint - temperature) > PREVENT_THERMAL_RUNAWAY_HYSTERESIS){
                    Serial.println("THERMAL_RUNAWAY_TEMP_FLAG");
                    if(THERMAL_RUNAWAY_TEMP_FLAG)
                        THERMAL_RUNAWAY_FLAG = true;
                    THERMAL_RUNAWAY_AT = millis();
                    THERMAL_RUNAWAY_TEMP_FLAG = !THERMAL_RUNAWAY_TEMP_FLAG;
                }
                else
                    THERMAL_RUNAWAY_TEMP_FLAG = false;
            }
        }
        else 
            THERMAL_RUNAWAY_FLAG=false;
        #endif
        EXTRUDER_SHOULD_RUN = !THERMAL_RUNAWAY_FLAG && !COLD_EXTRUSION_FLAG;
        
        if(!THERMAL_RUNAWAY_FLAG && HEATER_ENABLED)
            analogWrite(HEATER_PIN, output);
        else             
            analogWrite(HEATER_PIN, 0);
             
        
        vTaskDelay(ticks);  // one tick delay (15ms) in between reads for stability
    }
}

void TemperatureManager::setTemperature( double temperatureSetpoint){
    tempSetpoint = temperatureSetpoint;
    if(temperatureSetpoint > MAX_SET_TEMP)
        tempSetpoint = MAX_SET_TEMP;
    else if(temperatureSetpoint < 0)
        tempSetpoint = 0;
}

void TemperatureManager::incrementTemperature( int i ){
    setTemperature( tempSetpoint + i);
}