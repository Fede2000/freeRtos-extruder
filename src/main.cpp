#include <Arduino.h>
#include <EEPROM.h>
#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include <PID_v1.h>
#include "U8glib.h"
#include "displayUtility.h"
#include "configuration.h"
#include "eepromHelper.h"
#include "temperatureManager.h"
#include "menuManager.h"
#include "Extruder.h"


//double tempSetpoint = 35; // DEFAULT_TEMP=35; // TODO: eliminare default temperature


int ESetDefault=1000;
const float Ek = 1.0; // corrective factor

// consts for temperature measurement
float const R1 = 4700;
float const c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float logR2, R2, T;


/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/



TemperatureManager  temperatureManager  {	128, 2, "Temperature", 31};

void setup() {
  // serial init
  
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BUZZ_PIN,OUTPUT);
  digitalWrite(BUZZ_PIN,HIGH);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB  TODO: delete if serial port not used
  }
  if(EEPROM.read(ADDRESS_CK) == EEPROM_CK_VALUE){
    readEprom(temperatureManager.tempSetpoint, ESet);           
  }


  /* --------------------------------------Display settings -------------------------------------*/  
  //displaying logo
  u8g.firstPage();
  do {
    drawLogo();
  } while( u8g.nextPage() );
  delay(300);

  /* ------------------------------------ END Display settings ----------------------------------*/  

  
  // extruder settings
  extruder1.setMaxSpeed(1500);
  extruder1.setPinsInverted(false,false,true);  
  extruder1.setEnablePin(E_ENABLE_PIN);
  extruder1.setSpeed(ESet);
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
  
  /* --------------------------------------Timer4 settings-------------------------------------*/
  /* ------------ https://www.teachmemicro.com/arduino-timer-interrupt-tutorial/ ---------------*/
  /* ---https://www.google.com/search?q=timer+arduino+mega&newwindow=1&source=lnms&tbm=isch&sa=X&ved=0ahUKEwi1htP5v_rgAhWGx4UKHYSdDV8Q_AUIDygC&biw=1920&bih=969#imgrc=FSdu49NlAg2rrM: --*/
  // using a mask to protect other bits
  //noInterrupts();
  /*TIMSK2 = (TIMSK2 & B11111001) | 010;    // Time interrupt mask register
  TCCR2B = (TCCR2B & B11111000) | 010;   //prescaler: 8, freq = 16MHz --> 
  OCR2A = 100;    // compare register, callback every 1ms
  */
   TIMSK4 = (TIMSK4 & B11111101) | 0x06;
   TCCR4B = (TCCR4B & B11111000) | 0x02;   //prescaler 03
   OCR4A = 30; //62;   //1ms
   OCR4B = 5001;

   
  //interrupts();
  /* --------------------------------------------END------------------------------------------ */

 
  
  MenuManager menuManager {	512, 3, "Menu", 5, &temperatureManager};
  DisplayManager displayManager { 1524, 1, "Display", 100 / portTICK_PERIOD_MS, &menuManager, &temperatureManager};
  vTaskStartScheduler();
}

void loop() {
   // leave it empty
}


// callback for timer4 
ISR(TIMER4_COMPA_vect){
  TCNT4 = 0; // preload timer to 0
  #ifdef PREVENT_COLD_EXTRUSION 
    //if(temperatureManager.readTemperature() > EXTRUDE_MINTEMP) 
      extruder1.runSpeed();
  #endif
 }

 ISR(TIMER4_COMPB_vect){
 }