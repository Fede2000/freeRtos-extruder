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
    //menu 
    menu.addString("Status");
    menu.addString("Set");
    menu.addString("Save");
    menu.addString("Reset");
    //status
    status.addStringValue("Temp:", & (temperatureManager->temperature)); 
    status.addStringValue("Speed:", &extruderManager->speed_rpm); 
    //set
    set.addStringValue("Set temp: ", & (temperatureManager->tempSetpoint));
    set.addStringValue("Set speed: ", &extruderManager->speed_rpm);
    //save
    save.addString("*CONFIRM 1 click");
    save.addString("**BACK 2 clicks ");
    //reset
    reset.addString("*CONFIRM 1 click");
    reset.addString("**BACK 2 clicks ");
    Serial.println("DisplayManager");

}
 
void DisplayManager::Main(){
    for (;;) // A Task shall never return or exit.
    {
      menuManagerTest->updateMenu();
      
      u8g.firstPage();  
          do {
            menuManagerTest->ptMenu->drawMenu();
          } while( u8g.nextPage() );


      vTaskDelay(ticks);
  }
}