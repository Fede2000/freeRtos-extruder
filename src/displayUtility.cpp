#include "displayUtility.h"
#include "configuration.h"
#include "menuManager.h"
#include "temperatureManager.h"
#include "Extruder.h"
#include "Menu.h"





DisplayManager::DisplayManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks, MenuManager * aMenuManager, TemperatureManager * aTemperatureManager, Extruder * aExtruderManager ) : 
                                                                                    //menuManagerTest(aMenuManager),
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   


    Serial.println("DisplayManager");
    menuManagerTest = aMenuManager;
    temperatureManager = aTemperatureManager;
    extruderManager = aExtruderManager;

  
}
 
void DisplayManager::Main(){
    for (;;) // A Task shall never return or exit.
    {
      menuManagerTest->updateMenu();
      
      u8g.firstPage();  
          do {
            menuManagerTest->ptMenu->drawPage();
          } while( u8g.nextPage() );


      vTaskDelay(ticks);
  }
}