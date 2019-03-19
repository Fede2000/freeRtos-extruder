#include "TemperatureManager.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>

TemperatureManager  temperatureManager  {	128, 2, "Temperature", 31};
double tempSetpoint = 35;

TemperatureManager::TemperatureManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    myPID(&temperature, &output, &tempSetpoint, (double) CONST_KP, (double) CONST_KI, (double) CONST_KD, (int) 0 /*PID::DIRECT*/) , 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   
    myPID.SetMode(AUTOMATIC);
}

void TemperatureManager::getTemperature(){

    double average;
    average = (double) analogRead(THERMISTOR_PIN);
    average = 1023.0 / average - 1;
    average = (double) SERIESRESISTOR / average;
    /* TODO: implement average measurement 
    
    */
    // steinhart formula

    average = average / (double) THERMISTORNOMINAL;     // (R/Ro)
    average = log(average);                  // ln(R/Ro)
    average /= B_COEFFICIENT;                   // 1/B * ln(R/Ro)
    average += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    average = 1.0 / average;                 // Invert
    average -= 273.15;                         // convert to C
    temperature = average;

}
double TemperatureManager::readTemperature(){
    return temperature;
}

void TemperatureManager::Main() {
    for (;;)
    {
        getTemperature();
        myPID.Compute();
        analogWrite(HEATER_PIN, output); 
        vTaskDelay(ticks);  // one tick delay (15ms) in between reads for stability
    }
}