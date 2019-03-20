#include "U8glib.h"
#include "displayUtility.h"
#include "configuration.h"
#include "menuManager.h"
#include "temperatureManager.h"
#include "Extruder.h"



U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);
Menu menu(true, "MENU");
Menu status(false,"STATUS");
Menu set(true, "SETTINGS");
Menu save(false, "SAVE");
Menu reset(false, "RESET");
  
void Menu::drawMenu() 
{ 
    uint8_t i, h;
    u8g_uint_t w, d;
    int spacingTop = 3;
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();
    //draw title
    if(title !=NULL){
        u8g.setFont(u8g_font_7x13);
        d = (w- u8g.getStrWidth(title) )/2;
        u8g.drawStr(d, h + spacingTop, title);
        u8g.drawFrame(0,0,w, h + spacingTop*2);
        spacingTop = h + spacingTop*3;
    }

    u8g.setFont(u8g_font_6x10);
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();

    for( i = 0; i < itemIdx; i++ ) {
        d = (w-u8g.getStrWidth(menu_strings[i]))/2;
        u8g.setDefaultForegroundColor();
        if ( i == curruntMenu && isSelectable) {
            u8g.drawBox(d-2, spacingTop + i*h, w-2*d+2, h);
            u8g.setDefaultBackgroundColor();
        }
        u8g.drawStr(d, spacingTop+i*h, menu_strings[i]);
        
        if(menu_values_int[i] != NULL || menu_values_double[i]  != NULL ){
            u8g.setDefaultForegroundColor();
            u8g.setPrintPos(d -d*2 + w + 3, spacingTop + i*h+1);
            menu_values_int[i] != NULL ? u8g.print(*menu_values_int[i]) : u8g.print(*menu_values_double[i]);
        }   
    } 
} 


DisplayManager::DisplayManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks, MenuManager * aMenuManager, TemperatureManager * aTemperatureManager ) : 
                                                                                    //menuManagerTest(aMenuManager),
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   
    Serial.println("DisplayManager");
    menuManagerTest = aMenuManager;
    //menu 
    menu.addString("Status");
    menu.addString("Set");
    menu.addString("Save");
    menu.addString("Reset");
    //status
    status.addStringValue("Temp:", & (aTemperatureManager->temperature)); 
    status.addStringValue("Speed:", &ESet); 
    //set
    set.addStringValue("Set temp: ", & (aTemperatureManager->tempSetpoint));
    set.addStringValue("Set speed: ", &ESet);
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
      switch (menuManagerTest->page_current)
      {
        case 0:         //Menu
          Serial.println("oko");
          menuManagerTest->updateMenu();
          u8g.firstPage();  
          do {
            menu.drawMenu();
          } while( u8g.nextPage() );

        break;

        case 1:         //Status
          u8g.firstPage();
          do {
            status.drawMenu();
          } while( u8g.nextPage() );

        break;
        
        case 2:         //set
          u8g.firstPage();
          do {
            set.drawMenu();
          } while( u8g.nextPage() );

        break;

        case 3:         //save
          u8g.firstPage();
          do {
            save.drawMenu();
          } while( u8g.nextPage() );

        break;
        
        case 4:         //reset
          u8g.firstPage();
          do {
            reset.drawMenu();
          } while( u8g.nextPage() );

        break;

      }

      vTaskDelay(ticks);
  }
}