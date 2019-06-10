#ifndef	TEMPERATURE_MANAGER_h
#define TEMPERATURE_MANAGER_h


#include "configuration.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include "Thread.h"
#include "Arduino.h"


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class TemperatureManager : public Thread
{

private:
    
	uint32_t ticks;
    PID myPID;
    #ifdef PREVENT_THERMAL_RUNAWAY
        unsigned long THERMAL_RUNAWAY_AT = millis();
        double t1_temperature, t2_temperature;
    #endif

public:
    double output;
    double temperature;
    double tempSetpoint;
    float alpha;
    bool HEATER_ENABLED, PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = false, PREVENT_COLD_EXTRUSION_IS_ACTIVE = false, EXTRUDER_SHOULD_RUN,
    THERMAL_RUNAWAY_FLAG = false, COLD_EXTRUSION_FLAG=false;
    
    int stage = 0;  //HEATING_PHASE = 0, HOLDING_PHASE = 1, COOLING_PHASE = 2; 
    

    TemperatureManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks );

    void getTemperature();
    void setTemperature(double temperatureSetpoint);
    void incrementTemperature(int i);
    void setStage(); void initVariables();
    double readTemperature();
    void Main();


};

//extern TemperatureManager temperatureManager;

#endif

