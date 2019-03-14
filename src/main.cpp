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



/*--------------------------------------------------------------------*/
/*---------------------- Definitions & Variables ---------------------*/
/*--------------------------------------------------------------------*/
#define ENCODER_PIN1 31
#define ENCODER_PIN2 33
#define ENCODER_BTN 35

#define E_STEP_PIN 26
#define E_DIR_PIN 28
#define E_ENABLE_PIN 24

#define THERMISTOR_PIN 13
#define HEATER_PIN 10
double tempSetpoint = 35, tempDefault=35, steinhart;


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
uint8_t page_current = 1;  // status page       TODO: creare classe pagina -> menu
uint8_t menu_current = 0; 
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

Menu menu(&u8g);
Menu status(&u8g, false);
Menu set(&u8g);
Menu save(&u8g);
Menu reset(&u8g);




// define two tasks for Blink & AnalogRead
//void TaskExtruder( void *pvParameters );
void TaskTemperature( void *pvParameters );
void TaskEncoder( void *pvParameters );
void TaskDisplay( void *pvParameters );

void readEprom(int&, int&);
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


void timerIsr() { encoder.service(); } 

void setup() {
  // serial init
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB  TODO: delete if serial port not used
  }
  // encoder setup
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  encoder.setAccelerationEnabled(true);
  Serial.println(EEPROM.read(address_ck));
  //eeprom reading
  if(EEPROM.read(address_ck) == eeprom_ck_value){
    readEprom(tempSetpoint, ESet);
  }

  /* --------------------------------------Display settings -------------------------------------*/  
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);
  //menu 
  menu.addString("Status");
  menu.addString("Set");
  menu.addString("Save");
  menu.addString("Reset");
  //status
  status.addStringValue("Temperature: ", &steinhart);
  status.addStringValue("Speed: ", &ESet);
  //set
  set.addStringValue("Set temp: ", &tempSetpoint);
  set.addStringValue("Set speed: ", &ESet);
  //save
  save.addString("**confirm 1 click");
  save.addString("back 2 clicks: ");
  //reset
  reset.addString("**confirm 1 click");
  reset.addString("back 2 clicks: ");

  /* ------------------------------------ END Display settings ----------------------------------*/  

  // extruder settings
  extruder1.setMaxSpeed(1500);
  extruder1.setPinsInverted(false,false,true);  
  extruder1.setEnablePin(E_ENABLE_PIN);
  extruder1.setSpeed(ESet*Ek);
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
   TCCR4B = (TCCR4B & B11111000) | 0x03;   //prescaler
   OCR4A = 62; //62;   //1ms
   OCR4B = 5001;

   
  //interrupts();
  /* --------------------------------------------END------------------------------------------ */
   
  
  xTaskCreate(
    TaskTemperature
    ,  (const portCHAR *) "Temperature"
    ,  128 // This stack size can be checked & adjusted by reading Highwater
    ,  NULL
    ,  2  // priority
    ,  NULL );

  xTaskCreate(
    TaskEncoder
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
    ,  4  // priority
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
  /*if ( uiKeyCode != KEY_NONE && last_key_code == uiKeyCode ) {
    Serial.println("ss");
    return;
  }
  last_key_code = uiKeyCode;
  */
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
  //EEPROM.write(address_temp, temp);
  //EEPROM.write(address_speed, speed);
  write16b(temp,4);
  write16b(speed,8);
  EEPROM.write(address_ck, eeprom_ck_value);
}
void readEprom(int &temp, int &speed){
  //EEPROM.write(address_temp, temp);
  //EEPROM.write(address_speed, speed);
  temp = read16b(4);
  speed = read16b(8);
}







/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskTemperature(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  /**
   * thermistor and temperature settings
  **/
  #define THERMISTORNOMINAL 100000      
  // temp. for nominal resistance (almost always 25 C)
  #define TEMPERATURENOMINAL 25   
  // how many samples to take and average, more takes longer
  // but is more 'smooth'
  #define NUMSAMPLES 5
  // The beta coefficient of the thermistor (usually 3000-4000)
  #define B_COEFFICIENT 3950
  // the value of the RAMPS resistor
  #define SERIESRESISTOR 4700  
  float average;
  double Output;

  //Define PID Parameters
  double consKp=20, consKi=1.3, consKd=0.3;

  //Specify the links and initial tuning parameters
  PID myPID(&steinhart, &Output, &tempSetpoint, consKp, consKi, consKd, DIRECT);

  // initialize serial communication at 9600 bits per second:
  //Serial.begin(9600);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  for (;;)
  {
    
    average = analogRead(THERMISTOR_PIN);
    average = 1023 / average - 1;
    average = SERIESRESISTOR / average;
    /* TODO: implement average measurement 
    
    */
    steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= B_COEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C
  
   /* Serial.print("Temperature "); 
    Serial.print(steinhart);
    Serial.print(" *C");

    Serial.print(" Output: "); 
    Serial.print(Output);
    Serial.println(" *C");*/
    
    myPID.Compute();
    analogWrite(HEATER_PIN,Output ); 
    vTaskDelay(31);  // one tick delay (15ms) in between reads for stability
  }
}
void TaskEncoder(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
 
  // initialize digital pin 13 as an output.
  int tempEnc;
  for (;;) // A Task shall never return or exit.
  {
  
  buttonState = encoder.getButton();
  
  // if change speed mode:
  if (page_current == 2){
    if( setPageMenu ){
      set.curruntMenu = 1;
      ESet += encoder.getValue()*10;
      extruder1.setSpeed(ESet*Ek);
    }
    else{
      set.curruntMenu = 0;
      tempSetpoint += encoder.getValue();
    }    
  }
  // if change temperature mode:
  /*else if (lastButtonState == 6){
    tempSetpoint += encoder.getValue();
    Serial.print("**NEW tempSetpoint: "); Serial.println(tempSetpoint);
  }*/
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
        Serial.println("rel");
        if(page_current == 2){
          setPageMenu = !setPageMenu;
        }
        break;

      case ClickEncoder::Clicked:       //5
        if(page_current == 0)
          page_current= menu.curruntMenu + 1;
        //save
        else if(page_current == 3) {      
          page_current= 0;
          writeEprom(int(tempSetpoint), ESet);
        }
        else if(page_current == 4) {      
          page_current= 0;
          writeEprom(int(tempDefault), ESetDefault);
        }
        break;
        

      case ClickEncoder::DoubleClicked: //6
        page_current = 0;
        break;
    }
  }
  vTaskDelay(5);
  }
}

void TaskDisplay(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  

  Serial.println("dispA");
    
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
        Serial.println("set");
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

      /*default:        //Status
        //page_current = 1;
        u8g.firstPage();
        do {
          drawStatusPage();
        } while( u8g.nextPage() );

        break;*/
    }
    

  // send manual CR to the printer
 
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}


// callback for timer4 
ISR(TIMER4_COMPA_vect){
  TCNT4 = 0; // preload timer to 0 
  extruder1.runSpeed();
 }

 ISR(TIMER4_COMPB_vect){
 }



