#ifndef	DISPLAYUTILITY_h
#define DISPLAYUTILITY_h

#include "menuManager.h"
#include "Thread.h"
#include "temperatureManager.h"
#include "Extruder.h"
#include "Menu.h"

class DisplayManager : public Thread
{
    public:
        DisplayManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks, MenuManager * aMenuManager, TemperatureManager * aTemperatureManager, Extruder * extruderManager );
        void Main();

     private:
        MenuManager *menuManagerTest;
        TemperatureManager *temperatureManager;
        Extruder * extruderManager;
        uint32_t ticks;
  
};

#endif


