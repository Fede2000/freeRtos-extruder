#ifndef	TEMPERATURE_MANAGER_h
#define TEMPERATURE_MANAGER_h


#include "configuration.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include "Thread.h"


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class TemperatureManager : public Thread
{

private:
    double output;
	uint32_t ticks;
    PID myPID;
    #ifdef PREVENT_THERMAL_RUNAWAY
        int preventTR_Treshold;
        bool isHot;
    #endif

public:
    double temperature;
    double tempSetpoint;
    float alpha;

    TemperatureManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks );

    void getTemperature();
    void setTemperature(double temperatureSetpoint);
    void temperatureIncrement(int i);
    double readTemperature();
    void Main();


};

//extern TemperatureManager temperatureManager;

#endif

