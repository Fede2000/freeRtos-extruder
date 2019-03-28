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
    menuPage.addMenuString("Status");
    menuPage.addMenuString("Set");
    menuPage.addMenuString("Save");
    menuPage.addMenuString("Reset");
    menuPage.isSelectable = true;
    //status
    statusPage.addMenuStringValue("T:", & (temperatureManager->temperature)); 
    statusPage.addMenuStringValue("S:", &extruderManager->speed_rpm); 
    //set
    setPage.addMenuStringValue("Set temp: ", & (temperatureManager->tempSetpoint));
    setPage.addMenuStringValue("Set speed: ", &extruderManager->speed_rpm);
    setPage.isSelectable = true;
    //save
    savePage.addMenuString("*CONFIRM 1 click");
    savePage.addMenuString("**BACK 2 clicks ");
    //reset
    resetPage.addMenuString("*CONFIRM 1 click");
    resetPage.addMenuString("**BACK 2 clicks ");
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