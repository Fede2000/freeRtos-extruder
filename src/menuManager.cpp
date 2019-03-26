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
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   
    extruderManager = aExtruderManager;
    temperatureManagerTest = aTemperatureManager;
    ptMenu = &status;
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
        if( ptMenu->isSelected && ptMenu->curruntMenu == 0){
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
              ptMenu = ptPages[menu.curruntMenu + 1];
              ptMenu->isSelected = false;
            }
            //save
            else if(ptMenu->title == "SAVE") {      
              ptMenu = & menu;
              ptMenu->isSelected = false;
              writeEprom((int) temperatureManagerTest->tempSetpoint, (int) extruderManager->speed_rpm);
            }
            //reset
            else if(ptMenu->title == "RESET") {      
              ptMenu = & menu;
              ptMenu->isSelected = false;
              temperatureManagerTest->tempSetpoint = DEFAULT_TEMP; extruderManager->speed_rpm = DEFAULT_SPEED;
              writeEprom((int) DEFAULT_TEMP, (int) DEFAULT_SPEED);
            }
            else if(ptMenu->title == "STATUS") { 
              ptMenu = & menu;
            }
            break;
            
          case ClickEncoder::DoubleClicked: //6
            ptMenu->isSelected = ! ptMenu->isSelected;
            if(ptMenu->title != "MENU"){
              ptMenu = & menu;
              ptMenu->isSelected = false;
            } 
            else
            {
              ptMenu = & status;
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
        ptMenu->curruntMenu++;
        if ( ptMenu->curruntMenu >= ptMenu->itemIdx )
          ptMenu->curruntMenu = 0;
        //menu_redraw_required = 1;
        break;
      case KEY_PREV:
        uiKeyCode = KEY_NONE;
        if ( ptMenu->curruntMenu == 0 )
          ptMenu->curruntMenu = ptMenu->itemIdx;
        ptMenu->curruntMenu--;
        //menu_redraw_required = 1;
        break;
  }
}

