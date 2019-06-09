#include "menuManager.h"
#include "configuration.h"
#include "EepromHelper.h"
#include "Extruder.h"
#include "temperatureManager.h"
#include "logo.h"
#include <ClickEncoder.h>
#include <TimerOne.h>


ClickEncoder MenuManager::encoder(ENCODER_PIN1, ENCODER_PIN2, ENCODER_BTN, 4);
void timerIsr() { MenuManager::encoder.service(); } 

void drawLogo(){
  u8g2.drawXBMP( 0, 0, bmp_width, bmp_height, bmp_bits);
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

void MenuManager::saveEprom_short(){
  writeEprom((int) temperatureManagerTest->tempSetpoint, (int) extruderManager->speed_rpm, extruderManager->steps_to_retract, extruderManager->is_retraction_enabled);
}


void MenuManager::Main() {
    bool was_btn_stop = false, is_btn_stop = false, is_extruder_pin = false, was_extruder_pin = false,
      dataHasBeenSaved=true;
   
    for (;;)
    {
      encoderButtonState = encoder.getButton();
      is_btn_stop = !digitalRead(EXTRUDER_BTN_EN_PIN);
      is_extruder_pin = digitalRead(EXTRUDER_EN_PIN); 
      #ifdef STOP_BTN_TEST
        extruderManager->is_input_step = (digitalRead(EXTRUDER_BTN_EN_PIN) == 0) ||( digitalRead(EXTRUDER_EN_PIN) == 1);
        if(extruderManager->is_input_step == false && extruderManager->last_input_step){
          extruderManager->retract();
          Serial.println("retract");
        }EXTRUSION_ACCELERATION
        else if(extruderManager->is_input_step && extruderManager->last_input_step == false){
          extruderManager->overExtrude();
          Serial.println("overExtrude");
        }
      #endif //STOP_BTN_TEST
      #ifdef STOP_BTN_CONTROLL
        extruderManager->is_input_step = (is_btn_stop==0 && was_btn_stop==1 ) ? !extruderManager->is_input_step: extruderManager->is_input_step;  
        extruderManager->is_input_step = (is_extruder_pin==0 && was_extruder_pin==1 ) ? extruderManager->is_input_step= false: extruderManager->is_input_step = extruderManager->is_input_step; // TODO: else inutile
        extruderManager->is_input_step |= is_extruder_pin; 

        if(extruderManager->is_input_step == false && extruderManager->last_input_step){
          extruderManager->retract();
          Serial.println("retract");
        }
        else if(extruderManager->is_input_step && extruderManager->last_input_step == false){
          extruderManager->overExtrude();
          Serial.println("overExtrude");
        }
      #endif //STOP_BTN_CONTROLL

      extruderManager->last_input_step = extruderManager->is_input_step;
      was_btn_stop = is_btn_stop;
      was_extruder_pin = is_extruder_pin;

      //free the motor when not used
      digitalWrite(E_ENABLE_PIN,!((extruderManager->is_input_step || extruderManager->run_retraction)* temperatureManagerTest->EXTRUDER_SHOULD_RUN));
      
      if (ptMenu->title == "STATUS" && ptMenu->isSelected){
        dataHasBeenSaved=false;
        switch (ptMenu->currentMenu)
        {
        case 1:
          temperatureManagerTest->incrementTemperature(- encoder.getValue()); 
          temperatureManagerTest->setStage();
          break;
        case 3:
          extruderManager->incrementSpeed(- encoder.getValue());
          break;
        case 5:
          extruderManager->incrementRetraction(-encoder.getValue());
        
        default:
          break;
        }   

      }
      else if(!ptMenu->isSelected && !dataHasBeenSaved){
        saveEprom_short();
        dataHasBeenSaved=true;
      }
      
      else if (ptMenu->title == "SETTINGS" && ptMenu->isSelected){
        if(ptMenu->currentMenu == 0){
          temperatureManagerTest->incrementTemperature(- encoder.getValue());  
        }
        else {
          extruderManager->incrementSpeed(- encoder.getValue());
        }  
      }
      

      tempEnc += encoder.getValue();    
      if(tempEnc > 0)         {uiKeyCode = 1; tempEnc = 0;}
      else if(tempEnc < 0)   {uiKeyCode = -1; tempEnc = 0;}
      
      if (encoderButtonState != 0) {
        Serial.print("Button: "); Serial.println(encoderButtonState);
        lastButtonState = encoderButtonState;
        switch (encoderButtonState) {
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
            if(ptMenu->title == "STATUS") { 
              switch (ptMenu->currentMenu)
              {
                
                case 0:
                  temperatureManagerTest->HEATER_ENABLED = !temperatureManagerTest->HEATER_ENABLED;
                  ptMenu->heaterStatus = temperatureManagerTest->HEATER_ENABLED ? "HOT" : "COLD"; //TODO: heaterstatus non più utilizzato
                  temperatureManagerTest->setStage();
                  break;
                case 2:
                  extruderManager->is_input_step = !extruderManager->is_input_step;
                  break;

                case 4:
                  extruderManager->is_retraction_enabled = !extruderManager->is_retraction_enabled;
                  ptMenu->isSelected = false;
                  saveEprom_short();
                  break;

                /*case 3:
                  extruderManager->is_enabled = ! extruderManager->is_enabled;
                  ptMenu->motorStatus = extruderManager->is_enabled ? "ON" : "OFF";
                  break;
                */
                /*case 3:
                  ptMenu = & pages.menuPage;
                  ptMenu->isSelected = falsIT
                  break;
                
                case 5:
                  ptMenu->isSelected = false;
                  saveEprom_short();
                  break;
                */
                default:
                  ptMenu->isSelected = ! ptMenu->isSelected;
                  break;
              }
    

            }
            
            else if(ptMenu->title == "MENU"){
              ptMenu = pages.ptPages[pages.menuPage.currentMenu + 1];
              ptMenu->isSelected = false;
            }

            else if(ptMenu->title == "SETTINGS") { 
              switch (ptMenu->currentMenu)
              {
                case 2: //TRP
                  if(temperatureManagerTest->PREVENT_THERMAL_RUNAWAY_IS_ACTIVE){
                    temperatureManagerTest->PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = false;   
                    ptMenu->PTR = " no PTR";
                    temperatureManagerTest->THERMAL_RUNAWAY_FLAG = false;
                  }
                  else{
                    temperatureManagerTest->PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = true;
                    ptMenu->PTR =  "  PTR";
                  }    
                  ptMenu->isSelected = false;
                  break;

                case 3: //PCE
                  if(temperatureManagerTest->PREVENT_COLD_EXTRUSION_IS_ACTIVE){
                    temperatureManagerTest->PREVENT_COLD_EXTRUSION_IS_ACTIVE = false;   
                    ptMenu->PCE =  " no PCE";
                    temperatureManagerTest->COLD_EXTRUSION_FLAG = false;
                  }
                  else{
                    temperatureManagerTest->PREVENT_COLD_EXTRUSION_IS_ACTIVE = true;
                    ptMenu->PCE =  "  PCE";
                  }
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
                saveEprom_short();

              ptMenu = & pages.menuPage;
              ptMenu->isSelected = false;

            }
            //reset
            else if(ptMenu->title == "RESET") {      
              if(ptMenu->currentMenu == 0){
                temperatureManagerTest->tempSetpoint = DEFAULT_TEMP; extruderManager->speed_rpm = DEFAULT_SPEED;
                writeEprom((int) DEFAULT_TEMP, (int) DEFAULT_SPEED, (int) DEFAULT_RETRACTION_STEPS, true);
              }
              ptMenu = & pages.menuPage;
              ptMenu->isSelected = false;
            } 
            break;
          
          case ClickEncoder::DoubleClicked: //6
            //ptMenu->isSelected = ! ptMenu->isSelected;
            ptMenu->isSelected = false;
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

