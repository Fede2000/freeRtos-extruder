#include "Menu.h"
#include "temperatureManager.h"
#include "Extruder.h"
#include "U8glib.h"

class Pages 
{   
    public:    
        Page menuPage;
        StatusPage statusPage;
        SettingPage setPage;
        SavePage savePage;
        ResetPage resetPage; 
        Page *ptPages[5] = { &menuPage, &statusPage, &setPage, &savePage, &resetPage }; 
       
        Pages( TemperatureManager *aPtTemperatureManager, Extruder *aPtExtruderManager ): 
            menuPage("MENU"),
            statusPage("STATUS", aPtTemperatureManager, aPtExtruderManager),
            setPage("SETTINGS"),
            savePage("SAVE", u8g_font_6x12),
            resetPage("RESET", u8g_font_6x12)
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
            savePage.addMenuString("Save to eeprom");
            savePage.addMenuString("temperature & speed");
            //reset
            resetPage.addMenuString("Reset to default");
            resetPage.addMenuString("temperature & speed");
            
        }

    private:
        //TemperatureManager * ptTemperatureManager;

        
        
}; 