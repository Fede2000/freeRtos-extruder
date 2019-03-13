/**-----------------------------------------------**/
/*TODO:
-EEPROM
*/


#include <Arduino.h>
#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <PID_v1.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#include "U8glib.h"
#include "lcd.cpp"




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
double TempSetpoint = 35, steinhart;


int ESet = 1000;
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

/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/

//12864      
U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);



// define two tasks for Blink & AnalogRead
//void TaskExtruder( void *pvParameters );
void TaskTemperature( void *pvParameters );
void TaskEncoder( void *pvParameters );
void TaskDisplay( void *pvParameters );
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

  //display
  u8g.setFont(u8g_font_unifont);
  u8g.setColorIndex(1);

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
   
   /* Before a semaphore is used it must be explicitly created.  In this example
      a binary semaphore is created. */
  //xSemaphoreCreateBinary( xBinarySemaphore );
  //xBinarySemaphore = xSemaphoreCreateBinary();
  //xBinarySemaphore = xSemaphoreCreateMutex();
  //xSemaphoreGive( ( xBinarySemaphore ) );
  /*if ( xBinarySemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xBinarySemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xBinarySemaphore ) != NULL )
      xSemaphoreGive( ( xBinarySemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }*/
  //interrupts();
  // Now set up two tasks to run independently.
 /* xTaskCreate(
    TaskExtruder
    ,  (const portCHAR *)"Extruder"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  4  // priority
    ,  NULL );
*/
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
      menu_current++;
      if ( menu_current >= MENU_ITEMS )
        menu_current = 0;
      menu_redraw_required = 1;
      break;
    case KEY_PREV:
      uiKeyCode = KEY_NONE;
      if ( menu_current == 0 )
        menu_current = MENU_ITEMS;
      menu_current--;
      menu_redraw_required = 1;
      break;
  }
}

void drawMenu(void) {
  uint8_t i, h;
  u8g_uint_t w, d;

  u8g.setFont(u8g_font_6x13);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();
  
  h = u8g.getFontAscent()-u8g.getFontDescent();
  w = u8g.getWidth();
  for( i = 0; i < MENU_ITEMS; i++ ) {
    d = (w-u8g.getStrWidth(menu_strings[i]))/2;
    u8g.setDefaultForegroundColor();
    if ( i == menu_current ) {
      u8g.drawBox(d-2, i*h+1, -d*2 + w, h);
      u8g.setDefaultBackgroundColor();
    }
    u8g.drawStr(d, i*h, menu_strings[i]);
  }
}

void drawMenuPage(void)
{
	drawMenu();
}


void drawStatusPage(void)
{
  u8g.setFont(u8g_font_6x13);
  //u8g.setFontRefHeightText();
  //u8g.setFontPosTop();
  u8g.drawStr( 0, 20, "Speed: ");
  u8g.setPrintPos(80, 20);
  u8g.print(ESet);

  u8g.drawStr( 0, 40, "Temperature: ");
  u8g.setPrintPos(80, 40);
  u8g.print(int(steinhart));
}

void drawSetPage(void)
{
  u8g.setFont(u8g_font_orgv01);
  //u8g.setFontRefHeightText();
  //u8g.setFontPosTop();
  u8g.drawStr( 0, 20, "Set speed: ");
  u8g.setPrintPos(80, 20);
  u8g.print(ESet);

  u8g.drawStr( 0, 40, "Set temperature: ");
  u8g.setPrintPos(80, 40);
  u8g.print(TempSetpoint);
}


/* ----------------------------------------------------- */
/* ------------- END DISPLAY functions ----------------- */
/* ----------------------------------------------------- */



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
  PID myPID(&steinhart, &Output, &TempSetpoint, consKp, consKi, consKd, DIRECT);

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
      ESet += encoder.getValue()*10;
      extruder1.setSpeed(ESet*Ek);
    }
    else{
      TempSetpoint += encoder.getValue();
    }    
  }
  // if change temperature mode:
  /*else if (lastButtonState == 6){
    TempSetpoint += encoder.getValue();
    Serial.print("**NEW TempSetpoint: "); Serial.println(TempSetpoint);
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
          page_current=menu_current + 1;
          menu_current = 1;
        //else { page_current = 0; }
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
          drawMenuPage();
        } while( u8g.nextPage() );
        break;

      case 1:         //Status
        u8g.firstPage();
        do {
          drawStatusPage();
        } while( u8g.nextPage() );
        break;
      
      case 2:         //Status
        u8g.firstPage();
        do {
          drawSetPage();
        } while( u8g.nextPage() );
        break;

      default:        //Status
        //page_current = 1;
        u8g.firstPage();
        do {
          drawStatusPage();
        } while( u8g.nextPage() );
        break;
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



