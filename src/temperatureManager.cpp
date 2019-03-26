#include "TemperatureManager.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "configuration.h"

TemperatureManager::TemperatureManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    myPID(&temperature, &output, &tempSetpoint, (double) CONST_KP, (double) CONST_KI, (double) CONST_KD, (int) 0 /*PID::DIRECT*/) , 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{
    myPID.SetMode(AUTOMATIC);
    tempSetpoint = 35;
    alpha = 0.2;

    #ifdef PREVENT_THERMAL_RUNAWAY
        // Thermal Runaway protection settings
        preventTR_Treshold = 25; 
        isHot = false;
    #endif
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

}
double TemperatureManager::readTemperature(){
    return temperature;
}

void TemperatureManager::Main() {
    #ifdef PREVENT_THERMAL_RUNAWAY
        bool THERMAL_RUNAWAY_FLAG;
        unsigned long THERMAL_RUNAWAY_AT;
    #endif //PREVENT_THERMAL_RUNAWAY
    for (;;)
    {
        getTemperature();
        myPID.Compute();

        #ifdef PREVENT_THERMAL_RUNAWAY
            if( (millis() - THERMAL_RUNAWAY_AT) > 60000){
                if((tempSetpoint - temperature) > PREVENT_THERMAL_RUNAWAY_TRESHOLD){
                    if(THERMAL_RUNAWAY_FLAG)
                        Serial.println("STOP EVERYTHING!");
                    THERMAL_RUNAWAY_AT = millis();
                    THERMAL_RUNAWAY_FLAG = !THERMAL_RUNAWAY_FLAG;
                }
                else
                    THERMAL_RUNAWAY_FLAG = false;
            }
        /* =========== CODE =========== */
        #endif //PREVENT_THERMAL_RUNAWAY

        analogWrite(HEATER_PIN, output); 
        
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

void TemperatureManager::temperatureIncrement( int i ){
    setTemperature( tempSetpoint + i);
}