#include <Arduino.h>
#include "displayUtility.h"
#include "configuration.h"
#include "EepromHelper.h"
#include "temperatureManager.h"
#include "menuManager.h"
#include "Extruder.h"


#define compare_register OCR4A  //duty cycle


/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/


Extruder extruderManager;
TemperatureManager  temperatureManager  {	128, 1, "Temperature", 13}; //31  ticks
MenuManager menuManager {	512, 3, "Menu", 10, &temperatureManager, &extruderManager};
DisplayManager displayManager { 1524, 2, "Display", 100 / portTICK_PERIOD_MS, &menuManager, &temperatureManager, &extruderManager};

void setup() {
  // serial init
  Serial.begin(9600);

  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(E_DIR_PIN,OUTPUT);
  pinMode(BUZZ_PIN,OUTPUT);
  pinMode(EXTRUDER_BTN_EN_PIN, INPUT);
  
  digitalWrite(EXTRUDER_BTN_EN_PIN, HIGH);
  pinMode(EXTRUDER_EN_PIN, INPUT);
  
  pinMode(E_STEP_PIN,OUTPUT);
  //tone(BUZZ_PIN, 1000, 500);
  //delay(1000);
 /* while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB  TODO: delete if serial port not used
  }
  */
 
  if(EEPROM.read(ADDRESS_CK) == EEPROM_CK_VALUE){
    //Serial.println("loading eeprom values");
    readEprom(temperatureManager.tempSetpoint, extruderManager.speed_rpm,extruderManager.steps_to_retract, extruderManager.is_retraction_enabled);           
  }
 

  /* --------------------------------------Display settings -------------------------------------*/  
  u8g2.begin();
  //displaying logo
  #ifdef DRAW_LOGO
    u8g2.firstPage();
    do {
      drawLogo();
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(90, 60, VERSION);
    } while( u8g2.nextPage() );
    delay(2000);
  #endif

  /* ------------------------------------ END Display settings ----------------------------------*/  

  pinMode(E_ENABLE_PIN, OUTPUT);
  digitalWrite(E_ENABLE_PIN, HIGH);
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);
  
  /* --------------------------------------Timer4 settings-------------------------------------*/
  /* ------------ https://www.teachmemicro.com/arduino-timer-interrupt-tutorial/ ---------------*/
  /* ---https://www.google.com/search?q=timer+arduino+mega&newwindow=1&source=lnms&tbm=isch&sa=X&ved=0ahUKEwi1htP5v_rgAhWGx4UKHYSdDV8Q_AUIDygC&biw=1920&bih=969#imgrc=FSdu49NlAg2rrM: --*/
  /*
      int freq = 16 000 000;
      int prescaler = 8;

      period = 1/ (freq / prescaler ) * ticks;
      ticks = period * (freq / prescaler )
  */
  TCCR4A = 0;
  TCCR4B = 0;
  TCNT4 = 0; // initialize the counter from 0

  OCR4A = 26; //83 sets the counter compare value
  TCCR4A |= (1<<WGM41); // enable the CTC mode
  TCCR4B |= (1<<CS41) | (1<<CS40); // sets the control scale bits for the timer ....011 -> 64, 010 -> 8
  TIMSK4 |= (1<<OCIE4A); //enable the interrupt

/* --------------------------------------END Timer4 settings------------------------------------ */

/* ---------------------- start the OS scheduler ----------------- */
  vTaskStartScheduler();
}

void loop() {
   // leave it empty
}


// callback for timer4 
ISR(TIMER4_COMPA_vect){
  /* ----------------- TIMER ----------------------------*/
  /**
   * prescaler 011->64
   * system frequency = 16MHz
   * --> new frequency = 16/64 MHz
   * single tick period = 1 / new frequncy  = 4 * 10^(-6) [s]
   * 
  **/
  
  extruderManager.main();


  /* -------------------- MOTOR RUN ----------------------*/
  if(extruderManager.is_enabled && temperatureManager.EXTRUDER_SHOULD_RUN){
    if(extruderManager.is_input_step || extruderManager.run_retraction){
      // pin 26 = port A bit 4 --> step pin
      // 14 times faster
      //PORTA |= B00001000;
      //PORTA &= B11110111;
      //PORTA &= ~(1 << PORTA6);
      PORTA |= 1 << PORTA4;
      PORTA &= ~(1 << PORTA4);
      //extruderManager.steps++;
    }
  }
 }


 ISR(TIMER4_COMPB_vect){
   
 }
