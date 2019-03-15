#ifndef	TEMPERATURE_MANAGER_h
#define TEMPERATURE_MANAGER_h

#include "configuration.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>

class TemperatureManager {
    public:
    TemperatureManager(double * aTemperature, double * aTempSetpoint, const TickType_t xTicksToDelay = 31);
    float getTemperature();
    void init();
    void start( void *pvParameters);

    private:
    double output, *tempSetpoint, * temperature;
    const TickType_t xTicksToDelay;
    PID myPID(temperature, &output, tempSetpoint, CONST_KP, CONST_KI, CONST_KD, DIRECT);

}

