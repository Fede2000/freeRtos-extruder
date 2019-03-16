#include "menuManager.h"
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include <Arduino.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "eeprom_helper.h"
#include "configuration.h"


MenuManager::MenuManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks, double * aESet, double * aTempSetpoint, AccelStepper & aExtruder ) : 
                                                                                    encoder(ENCODER_PIN1, ENCODER_PIN2, ENCODER_BTN, 4) , 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   
    ESet = aESet;
    tempSetpoint = aTempSetpoint;
    extruder = aExtruder;
    

    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    encoder.setAccelerationEnabled(true);

}

void MenuManager::timerIsr() { encoder.service(); } 

void MenuManager::Main() {
    for (;;)
    {
        buttonState = encoder.getButton();
    
    // if change speed mode:
    if (page_current == Settings){
      if( setPageMenu ){
        set.curruntMenu = 1;
        ESet += encoder.getValue();
        extruder.setSpeed(ESet); // TODO:Ek
      }
      else{
        set.curruntMenu = 0;
        tempSetpoint += encoder.getValue();
      }    
    }

    else{
      tempEnc += encoder.getValue();    // TODO: return if zero
      if(tempEnc > 1)         {uiKeyCode = 1; tempEnc = 0;}
      else if(tempEnc < -1)   {uiKeyCode = -1; tempEnc = 0;}
    }
    
    // TODO: provare a inserirle nell switch

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
          if(page_current == Settings){
            setPageMenu = !setPageMenu;
          }
          break;

        case ClickEncoder::Clicked:       //5
          if(page_current == Menu_p)
            page_current = (Page_t) (menu.curruntMenu + 1);
          //save
          else if(page_current == Save) {      
            page_current = Menu_p;
            writeEprom(int(tempSetpoint), ESet);
          }
          //reset
          else if(page_current == Reset) {      
            page_current = Menu_p;
            tempSetpoint = DEFAULT_TEMPERATURE; ESet = DEFAULT_SPEED;
            writeEprom(int(DEFAULT_TEMPERATURE), DEFAULT_SPEED);
          }
          break;
          
        case ClickEncoder::DoubleClicked: //6
          page_current = Menu_p;
          break;
      }
    }
    vTaskDelay(ticks);
    }
}