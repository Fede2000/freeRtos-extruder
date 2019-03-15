#include "TemperatureManager.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>

TemperatureManager::TemperatureManager(double * aTemperature, double * aInput, double * aTempSetpoint, const TickType_t aTicksToDelay = 31){
    temperature = aTemperature;
    input = aInput;
    xTicksToDelay = aTicksToDelay;
    tempSetpoint = aTempSetpoint;
}
void TemperatureManager::init(){
    myPID.SetMode(AUTOMATIC);
}

float TemperatureManager::getTemperature(){
    temperature = analogRead(THERMISTOR_PIN);
    temperature = 1023 / temperature - 1;
    temperature = SERIESRESISTOR / temperature;
    /* TODO: implement average measurement 
    
    */
    // steinhart formula
    temperature = temperature / THERMISTORNOMINAL;     // (R/Ro)
    temperature = log(temperature);                  // ln(R/Ro)
    temperature /= B_COEFFICIENT;                   // 1/B * ln(R/Ro)
    temperature += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    temperature = 1.0 / temperature;                 // Invert
    temperature -= 273.15;                         // convert to C
    return temperature;
}
void start(void *pvParameters){
    for (;;)
    {
        getTemperature();
        myPID.Compute();
        analogWrite(HEATER_PIN, *Output); 
        vTaskDelay(xTicksToDelay);  // one tick delay (15ms) in between reads for stability
    }
}