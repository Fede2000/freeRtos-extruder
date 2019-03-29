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


const float Ek = 1.0; // corrective factor

// consts for temperature measurement
float const R1 = 4700;
float const c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float logR2, R2, T;

#define compare_register OCR4A  //duty cycle


/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/


//Extruder extruderManager {	128, 4, "Temperature", 11}; //Highest priority, TODO: è inutile farlo girare qdo la velocità target è stata raggiunta
Extruder extruderManager;
TemperatureManager  temperatureManager  {	128, 2, "Temperature", 31};
MenuManager menuManager {	512, 3, "Menu", 5, &temperatureManager, &extruderManager};
void setup() {
  // serial init
  
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BUZZ_PIN,OUTPUT);
  pinMode(EN_M_PIN, INPUT);
  digitalWrite(EN_M_PIN, HIGH);
  pinMode(EN_PIN, INPUT);

  pinMode(E_STEP_PIN,OUTPUT);

  digitalWrite(BUZZ_PIN,HIGH);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB  TODO: delete if serial port not used
  }
  
  if(EEPROM.read(ADDRESS_CK) == EEPROM_CK_VALUE){
    readEprom(temperatureManager.tempSetpoint, extruderManager.speed_rpm);           
  }
 

  /* --------------------------------------Display settings -------------------------------------*/  
  //displaying logo
  u8g.firstPage();
  do {
    drawLogo();
  } while( u8g.nextPage() );
  delay(200);

  /* ------------------------------------ END Display settings ----------------------------------*/  

  
  // extruder settings
  extruder1.setMaxSpeed(1500);
  extruder1.setPinsInverted(false,false,true);  
  extruder1.setEnablePin(E_ENABLE_PIN);
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
   //TIMSK4 = (TIMSK4 & B11111101) | 0x06;
   //TCCR4A = (TCCR4A & B11111000) | 0x03;   //prescaler 02  // in caso di modifica: modificare anche...
   //OCR4A = 26; //83
   //OCR4B = 5001;

  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0; // initialize the counter from 0

  OCR4A = 26; //83 sets the counter compare value
  TCCR4A |= (1<<WGM41); // enable the CTC mode
  TCCR4B |= (1<<CS41) | (1<<CS40); // sets the control scale bits for the timer ....011 -> 64
  TIMSK4 |= (1<<OCIE4A); //enable the interrupt



/*
int freq = 16 000 000;
int prescaler = 8;

period = 1/ (freq / prescaler ) * ticks;
ticks = period * (freq / prescaler )

*/
  //interrupts();
  /* --------------------------------------------END------------------------------------------ */

 
  
  
  DisplayManager displayManager { 1524, 1, "Display", 100 / portTICK_PERIOD_MS, &menuManager, &temperatureManager, &extruderManager};
  vTaskStartScheduler();
}

void loop() {
   // leave it empty
}


// callback for timer4 
ISR(TIMER4_COMPA_vect){

  //TCNT4 = 0; // preload timer to 0
  //compare_register = extruderManager.timer;
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0; // initialize the counter from 0
  extruderManager.runSpeed();
  OCR4A = extruderManager.timer; //83 sets the counter compare value
  TCCR4A |= (1<<WGM41); // enable the CTC mode
  TCCR4B |= (1<<CS41) | (1<<CS40); // sets the control scale bits for the timer ....011 -> 64
  TIMSK4 |= (1<<OCIE4A); //enable the interrupt
  

  #ifdef PREVENT_COLD_EXTRUSION
     
    if(temperatureManager.readTemperature() > EXTRUDE_MIN_EXTRUSION_TEMP){
      if(extruderManager.is_step && extruderManager.is_enabled){
        digitalWrite(E_STEP_PIN, HIGH);
        digitalWrite(E_STEP_PIN, LOW);
      }
    }
  #endif


 }

 ISR(TIMER4_COMPB_vect){
 }
 