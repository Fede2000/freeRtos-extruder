#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "EepromHelper.h"
#include "configuration.h"
#include "displayUtility.h"
#include "U8glib.h"
#include "logo.h"
#include "menuManager.h"


Menu menu(true, "MENU");
Menu status(false,"STATUS");
Menu set(true, "SETTINGS");
Menu save(false, "SAVE");
Menu reset(false, "RESET");

ClickEncoder MenuManager::encoder(ENCODER_PIN1, ENCODER_PIN2, ENCODER_BTN, 4);
void timerIsr() { MenuManager::encoder.service(); } 

void drawLogo(){
  u8g.drawXBMP( 0, 0, bmp_width, bmp_height, bmp_bits);
}

MenuManager::MenuManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, 
                        uint32_t _ticks, int * aESet, double * aTempSetpoint, AccelStepper * aExtruder ) :                                                                                                                                 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{   

    ESet = aESet;
    tempSetpoint = aTempSetpoint;
    extruder = aExtruder;
    

    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);
    encoder.setAccelerationEnabled(true);

    //menu 
    menu.addString("Status");
    menu.addString("Set");
    menu.addString("Save");
    menu.addString("Reset");
    //status
    status.addStringValue("Temp:", tempSetpoint); //TODO: &(temperatureManager.temperature)
    status.addStringValue("Speed:", ESet);  //&ESet
    //set
    set.addStringValue("Set temp: ", tempSetpoint);
    set.addStringValue("Set speed: ", ESet);
    //save
    save.addString("*CONFIRM 1 click");
    save.addString("**BACK 2 clicks ");
    //reset
    reset.addString("*CONFIRM 1 click");
    reset.addString("**BACK 2 clicks ");


}



void MenuManager::Main() {
    for (;;)
    {
        buttonState = encoder.getButton();
    
    // if change speed mode:
    if (page_current == Settings){
      if( setPageMenu ){
        set.curruntMenu = 1;
        * ESet += encoder.getValue();
        extruder->setSpeed(* ESet); // TODO:Ek
      }
      else{
        set.curruntMenu = 0;
        *tempSetpoint += encoder.getValue();
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
            writeEprom(int(tempSetpoint), *ESet);
          }
          //reset
          else if(page_current == Reset) {      
            page_current = Menu_p;
            *tempSetpoint = DEFAULT_TEMP; *ESet = DEFAULT_SPEED;
            writeEprom(int(DEFAULT_TEMP), DEFAULT_SPEED);
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


void MenuManager::updateMenu() {
 
    switch ( uiKeyCode ) {
      case KEY_NEXT:
        uiKeyCode = KEY_NONE;
        menu.curruntMenu++;
        if ( menu.curruntMenu >= menu.itemIdx )
          
          menu.curruntMenu = 0;
        //menu_redraw_required = 1;
        break;
      case KEY_PREV:
        uiKeyCode = KEY_NONE;
        if ( menu.curruntMenu == 0 )
          menu.curruntMenu = menu.itemIdx;
        menu.curruntMenu--;
        //menu_redraw_required = 1;
        break;
  }
}

