#include "Menu.h"
#include "temperatureManager.h"
#include "Extruder.h"

class Pages 
{   
    public:    
        Page menuPage;
        StatusPage statusPage;
        Page setPage;
        SavePage savePage;
        ResetPage resetPage; 
        Page *ptPages[5] = { &menuPage, &statusPage, &setPage, &savePage, &resetPage }; 
       
        Pages( TemperatureManager *aPtTemperatureManager, Extruder *aPtExtruderManager ): 
            menuPage("MENU"),
            statusPage("STATUS", aPtTemperatureManager),
            setPage("SETTINGS"),
            savePage("SAVE"),
            resetPage("RESET")
        { 
              //menu 
            menuPage.addMenuString("Status");
            menuPage.addMenuString("Set");
            menuPage.addMenuString("Save");
            menuPage.addMenuString("Reset");
            menuPage.isSelectable = true;
            //status
            statusPage.addMenuStringValue("T:", & (aPtTemperatureManager->temperature)); 
            statusPage.addMenuStringValue("S:", &aPtExtruderManager->speed_rpm); 
            //set
            setPage.addMenuStringValue("Set temp: ", & (aPtTemperatureManager->tempSetpoint));
            setPage.addMenuStringValue("Set speed: ", & aPtExtruderManager->speed_rpm);
            setPage.isSelectable = true;
            //save
            savePage.addMenuString("*CONFIRM 1 click");
            savePage.addMenuString("**BACK 2 clicks ");
            //reset
            resetPage.addMenuString("*CONFIRM 1 click");
            resetPage.addMenuString("**BACK 2 clicks ");
        }

    private:
        //TemperatureManager * ptTemperatureManager;

        
        
}; 