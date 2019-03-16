/**-----------------------------------------------**/
/*TODO:
-EEPROM
*/


#include <Arduino.h>
#include <EEPROM.h>

#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <PID_v1.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#include "U8glib.h"
#include "displayUtility.h"
#include "logo.h"
#include "configuration.h"
#include "temperatureManager.h"



double tempSetpoint = 35, DEFAULT_TEMPERATURE=35;


int ESet = 1000, ESetDefault=1000;
const float Ek = 1.0; // corrective factor

// consts for temperature measurement
float const R1 = 4700;
float const c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float logR2, R2, T;

// encoder
uint8_t buttonState, lastButtonState;
int encPos;
// display
Page_t page_current = Status;  // status page       TODO: creare classe pagina -> menu
bool setPageMenu = true;  

//eeprom
int address_temp = 0;
int address_speed = 1;
int address_ck = 2;
int eeprom_ck_value = 12;

/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/

//12864      
U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);

Menu menu(&u8g,true, "MENU");
Menu status(&u8g, false,"STATUS");
Menu set(&u8g, true, "SETTINGS");
Menu save(&u8g,false, "SAVE");
Menu reset(&u8g, false, "RESET");




// define two tasks for Blink & AnalogRead
//void TaskExtruder( void *pvParameters );
//void TaskTemperature( void *pvParameters );
TemperatureManager temperatureManager{	128, 2, "Temperature", 31, &tempSetpoint};

void TaskMenu( void *pvParameters );
void TaskDisplay( void *pvParameters );

void readEprom(double&, int&);

void drawLogo();
/* The service routine for the interrupt.  This is the interrupt that the task
will be synchronized with. */
//static void vExampleInterruptHandler( void );

/* Declare a variable of type SemaphoreHandle_t.  This is used to reference the
semaphore that is used to synchronize a task with an interrupt. */
//SemaphoreHandle_t xBinarySemaphore;


/* ------------------------------------------------- */


// extruder1: instance of AccelStepper 0
AccelStepper extruder1(1, E_STEP_PIN, E_DIR_PIN);
// encoder
ClickEncoder encoder(ENCODER_PIN1, ENCODER_PIN2, ENCODER_BTN, 4);
/*void x(){
     temperatureManager.run(NULL);
   }*/

void timerIsr() { encoder.service(); } 

void setup() {
  // serial init
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(BUZZ_PIN,OUTPUT);
  digitalWrite(BUZZ_PIN,HIGH);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB  TODO: delete if serial port not used
  }
  // encoder setup
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  encoder.setAccelerationEnabled(true);
  //eeprom reading
  if(EEPROM.read(address_ck) == eeprom_ck_value){
    readEprom(tempSetpoint, ESet);           
  }

  /* --------------------------------------Display settings -------------------------------------*/  
  //displaying logo
  u8g.firstPage();
  do {
    drawLogo();
  } while( u8g.nextPage() );
  delay(300);
  //Serial.println(temperatureManager.temperature);
  //menu 
  menu.addString("Status");
  menu.addString("Set");
  menu.addString("Save");
  menu.addString("Reset");
  //status
  status.addStringValue("Temp:", &(temperatureManager.temperature));
  status.addStringValue("Speed:", &ESet);
  //set
  set.addStringValue("Set temp: ", &tempSetpoint);
  set.addStringValue("Set speed: ", &ESet);
  //save
  save.addString("*CONFIRM 1 click");
  save.addString("**BACK 2 clicks ");
  //reset
  reset.addString("*CONFIRM 1 click");
  reset.addString("**BACK 2 clicks ");

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
   /*
  xTaskCreate(
    x
    ,  (const portCHAR *) "Temperature"
    ,  4096 // This stack size can be checked & adjusted by reading Highwater
    ,  NULL
    ,  2  // priority
    ,  NULL );*/

  //TemperatureManager temperatureManager{	128, 2, "Temperature", 31, &tempSetpoint};

  xTaskCreate(
    TaskMenu
    ,  (const portCHAR *)"Encoder"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  3  // priority
    ,  NULL );

  xTaskCreate(
    TaskDisplay
    ,  (const portCHAR *)"Display"   // A name just for humans
    ,  1524  // Stack size
    ,  NULL
    ,  1  // priority
    ,  NULL );
}

void loop() {
   // leave it empty
}


/* ----------------------------------------------------- */
/* --------------- DISPLAY functions ------------------- */
/* ----------------------------------------------------- */
#define MENU_ITEMS 4
#define KEY_NONE 0
#define KEY_NEXT 1
#define KEY_PREV -1

char *menu_strings[MENU_ITEMS] = { "Status", "Set", "Save", "Reset" };
int *menu_values[MENU_ITEMS] = { 1,2,3,4 };

uint8_t menu_redraw_required = 0;   // not used yet
uint8_t last_key_code = KEY_NONE;
int uiKeyCode = KEY_NONE;
 

void updateMenu(void) {

  switch ( uiKeyCode ) {
    case KEY_NEXT:
      uiKeyCode = KEY_NONE;
      menu.curruntMenu++;
      if ( menu.curruntMenu >= menu.itemIdx )
        
        menu.curruntMenu = 0;
      menu_redraw_required = 1;
      break;
    case KEY_PREV:
      uiKeyCode = KEY_NONE;
      if ( menu.curruntMenu == 0 )
        menu.curruntMenu = menu.itemIdx;
      menu.curruntMenu--;
      menu_redraw_required = 1;
      break;
  }
}
void drawLogo(){
  u8g.drawXBMP( 0, 0, bmp_width, bmp_height, bmp_bits);
}


/* ----------------------------------------------------- */
/* ------------- END DISPLAY functions ----------------- */
/* ----------------------------------------------------- */


// EEPROM
write16b( int value, int addr = 0){
  char * pt = (char *) &value;
  EEPROM.write(addr++, *pt++);
  EEPROM.write(addr , *pt );
}
int read16b(int addr){
  int dato = 0;
  char * pt = (char *) &dato;
  *pt++ = EEPROM.read(addr++);
  *pt++ = EEPROM.read(addr);
   return dato;
}

void writeEprom(int temp, int speed){
  write16b(temp,4);
  write16b(speed,8);
  EEPROM.write(address_ck, eeprom_ck_value);
}
void readEprom(double &temp, int &speed){
  temp = read16b(4);
  speed = read16b(8);
}

/*
#if ENABLED(PREVENT_COLD_EXTRUSION)
*
*
*
*
#endif
*/



/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/


void TaskMenu(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
 
  // initialize digital pin 13 as an output.
  int tempEnc;
  for (;;) // A Task shall never return or exit.
  {
  
    buttonState = encoder.getButton();
    
    // if change speed mode:
    if (page_current == Settings){
      if( setPageMenu ){
        set.curruntMenu = 1;
        ESet += encoder.getValue();
        extruder1.setSpeed(ESet*Ek);
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
            tempSetpoint = DEFAULT_TEMPERATURE; ESet = ESetDefault;
            writeEprom(int(DEFAULT_TEMPERATURE), ESetDefault);
          }
          break;
          
        case ClickEncoder::DoubleClicked: //6
          page_current = Menu_p;
          break;
      }
    }
    vTaskDelay(5);
  }
}

void TaskDisplay(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
      
  for (;;) // A Task shall never return or exit.
  {
    
    switch (page_current)
    {
      case 0:         //Menu
        updateMenu();
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
    

  // send manual CR to the printer
 
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}


// callback for timer4 
ISR(TIMER4_COMPA_vect){
  TCNT4 = 0; // preload timer to 0
  #ifdef PREVENT_COLD_EXTRUSION 
    //if(temperatureManager.readTemperature() > EXTRUDE_MINTEMP) // da migliorare
      extruder1.runSpeed();
  #endif
 }

 ISR(TIMER4_COMPB_vect){
 }



