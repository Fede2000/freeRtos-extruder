#ifndef	TEMPERATURE_MANAGER_h
#define TEMPERATURE_MANAGER_h


#include "configuration.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include "Thread.h"


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class TemperatureManager : public Thread
{
public:
    TemperatureManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks );

    double temperature;
    void getTemperature();
    double readTemperature();
    virtual void Main() override;

private:
    double output;
	uint32_t ticks;
    PID myPID;
};

extern double tempSetpoint;
extern TemperatureManager temperatureManager;

#endif

