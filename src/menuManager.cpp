#include "menuManager.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "EepromHelper.h"
#include "configuration.h"
#include "logo.h"
#include "Extruder.h"
#include "temperatureManager.h"


ClickEncoder MenuManager::encoder(ENCODER_PIN1, ENCODER_PIN2, ENCODER_BTN, 4);
void timerIsr() { MenuManager::encoder.service(); } 

void drawLogo(){
  u8g.drawXBMP( 0, 0, bmp_width, bmp_height, bmp_bits);
}

MenuManager::MenuManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, 
                        uint32_t _ticks, TemperatureManager *  aTemperatureManager, Extruder * aExtruderManager) :
                                                                                    //temperatureManagerTest(aTemperatureManager),
                                                                                    pages(aTemperatureManager, aExtruderManager),
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   
    extruderManager = aExtruderManager;
    temperatureManagerTest = aTemperatureManager;

    ptMenu = &pages.statusPage;
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    encoder.setAccelerationEnabled(true);

}


void MenuManager::Main() {
    //Serial.println("qwerty");
    for (;;)
    {
      buttonState = encoder.getButton();
      extruderManager->is_step = (digitalRead(EN_M_PIN) == 0) ||( digitalRead(EN_PIN) == 1);

      if (ptMenu->title == "SETTINGS"){
        if( ptMenu->isSelected && ptMenu->currentMenu == 0){
          temperatureManagerTest->temperatureIncrement(- encoder.getValue());  
        }
        else if(ptMenu->isSelected){
          extruderManager->incrementSpeed(- encoder.getValue());
        }  
      }

      //else{
        tempEnc += encoder.getValue();    
        if(tempEnc > 0)         {uiKeyCode = 1; tempEnc = 0;}
        else if(tempEnc < 0)   {uiKeyCode = -1; tempEnc = 0;}
      //}
      
      if (buttonState != 0) {
        Serial.print("Button: "); Serial.println(buttonState);
        lastButtonState = buttonState;
        switch (buttonState) {
          case ClickEncoder::Open:          //0
            break;

          case ClickEncoder::Closed:        //1
            break;

          case ClickEncoder::Pressed:       //2
            break;

          case ClickEncoder::Held:          //3
            break;
      

          case ClickEncoder::Released:      //4
            break;

          case ClickEncoder::Clicked:       //5
            ptMenu->isSelected = ! ptMenu->isSelected;
            if(ptMenu->title == "MENU"){
              ptMenu = pages.ptPages[pages.menuPage.currentMenu + 1];
              ptMenu->isSelected = false;
            }

            else if(ptMenu->title == "SETTINGS") { 
              switch (ptMenu->currentMenu)
              {
                case 2: //TRP
                  #ifdef PREVENT_THERMAL_RUNAWAY
                      #undef PREVENT_THERMAL_RUNAWAY
                      ptMenu->PTR =  " no PTR";
                      temperatureManagerTest->THERMAL_RUNAWAY_FLAG = false;
                  #endif
                  #ifndef PREVENT_THERMAL_RUNAWAY
                    #define PREVENT_THERMAL_RUNAWAY
                    ptMenu->PTR =  "  PTR";
                  #endif
    
                  ptMenu->isSelected = false;
                  break;

                case 3: //PCE
                  #ifdef PREVENT_COLD_EXTRUSION
                      #undef PREVENT_COLD_EXTRUSION
                      ptMenu->PCE =  " no PCE";
                      temperatureManagerTest->COLD_EXTRUSION_FLAG = false;
                  #endif
                  #ifndef PREVENT_COLD_EXTRUSION
                    #define PREVENT_COLD_EXTRUSION
                    ptMenu->PCE =  "  PCE";
                  #endif
                  ptMenu->isSelected = false;
                  break;

                case 4:
                  ptMenu = & pages.menuPage;
                  ptMenu->isSelected = false;
                  break;

                default:
                  break;
              }
            }
            //save
            else if(ptMenu->title == "SAVE") {
              if(ptMenu->currentMenu == 0)
                writeEprom((int) temperatureManagerTest->tempSetpoint, (int) extruderManager->speed_rpm);

              ptMenu = & pages.menuPage;
              ptMenu->isSelected = false;

            }
            //reset
            else if(ptMenu->title == "RESET") {      
              if(ptMenu->currentMenu == 0){
                temperatureManagerTest->tempSetpoint = DEFAULT_TEMP; extruderManager->speed_rpm = DEFAULT_SPEED;
                writeEprom((int) DEFAULT_TEMP, (int) DEFAULT_SPEED);
              }
              ptMenu = & pages.menuPage;
              ptMenu->isSelected = false;
            }
            else if(ptMenu->title == "STATUS") { 
              switch (ptMenu->currentMenu)
              {
                case 0:
                  temperatureManagerTest->HEATER_ENABLED = !temperatureManagerTest->HEATER_ENABLED;
                  ptMenu->heaterStatus = temperatureManagerTest->HEATER_ENABLED ? "HOT" : "COLD";
          
                  break;
                case 1:
                  extruderManager->is_enabled = ! extruderManager->is_enabled;
                  ptMenu->motorStatus = extruderManager->is_enabled ? "ON" : "OFF";
                  break;
                case 2:
                  ptMenu = & pages.menuPage;
                  ptMenu->isSelected = false;
                  break;

                default:
                  break;
              }
    

            }
            break;
            
          case ClickEncoder::DoubleClicked: //6
            ptMenu->isSelected = ! ptMenu->isSelected;
            if(ptMenu->title != "MENU"){
              ptMenu = & pages.menuPage;
              ptMenu->isSelected = false;
            } 
            else
            {
              ptMenu = & pages.statusPage;
            }
            
            break;
        }
      }
      
      vTaskDelay(ticks);
    }
}


void MenuManager::updateMenu() {
 
    switch ( uiKeyCode ) {
      
      case KEY_NEXT:
        uiKeyCode = KEY_NONE;
        ptMenu->currentMenu++;
        if ( ptMenu->currentMenu >= ptMenu->nMenuItems)
          ptMenu->currentMenu = 0;
        //menu_redraw_required = 1;
        break;
      case KEY_PREV:
        uiKeyCode = KEY_NONE;
        if ( ptMenu->currentMenu == 0 )
          ptMenu->currentMenu = ptMenu->nMenuItems;
        ptMenu->currentMenu--;
        //menu_redraw_required = 1;
        break;
  }
}

