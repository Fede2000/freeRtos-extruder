#include <Arduino.h>
#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <PID_v1.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif


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
int16_t oldEncPos, encPos;
uint8_t buttonState, lastButtonState;

/*--------------------------------------------------------------------*/
/*------------------- END Definitions & Variables --------------------*/
/*--------------------------------------------------------------------*/

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE); 	      


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
SemaphoreHandle_t xBinarySemaphore;


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
  oldEncPos = -1;
  encoder.setAccelerationEnabled(true);

  //display
  u8x8.begin();


  // extruder settings
  extruder1.setMaxSpeed(1000000);
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
   TCCR4B = (TCCR4B & B11111000) | 0x03;
   OCR4A = 100;
   OCR4B = 100;

   
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
    ,  3  // priority
    ,  NULL );

  xTaskCreate(
    TaskEncoder
    ,  (const portCHAR *)"Encoder"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  2  // priority
    ,  NULL );

  /*xTaskCreate(
    TaskDisplay
    ,  (const portCHAR *)"Display"   // A name just for humans
    ,  128  // Stack size
    ,  NULL
    ,  1  // priority
    ,  NULL );*/
}

void loop() {
   // leave it empty
}


/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
/*
void TaskExtruder(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.
  #define LONG_TIME 0xffff

  for (;;) // A Task shall never return or exit.
  {

    //xSemaphoreTake( xBinarySemaphore, ( TickType_t ) portMAX_DELAY );   // continuosly check for portMAX_DELAY
    if( xSemaphoreTake( xBinarySemaphore, 10 ) == pdTRUE ){
      extruder1.runSpeed();
    }
    //extruder1.runSpeed();
    //vTaskDelay(10); // wait for one second
    //xSemaphoreGive( xBinarySemaphore );
  }
}*/

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
  
    Serial.print("Temperature "); 
    Serial.print(steinhart);
    Serial.print(" *C");

    Serial.print(" Output: "); 
    Serial.print(Output);
    Serial.println(" *C");
    
    myPID.Compute();
    analogWrite(HEATER_PIN, Output);
    vTaskDelay(30);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskEncoder(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.

  for (;;) // A Task shall never return or exit.
  {
 
  buttonState = encoder.getButton();

  // if change speed mode:
  if (lastButtonState == 5){
    ESet += encoder.getValue()*10;
    extruder1.setSpeed(ESet*Ek);
    Serial.print("**NEW ESet: "); Serial.println(ESet);
  }
  // if change temperature mode:
  else if (lastButtonState == 6){
    TempSetpoint += encoder.getValue();
    Serial.print("**NEW TempSetpoint: "); Serial.println(TempSetpoint);
  }
  // TODO: provare a inserirle nell switch
  //encPos += encoder.getValue();

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
        break;

      case ClickEncoder::DoubleClicked: //6
        break;
    }
  }
    vTaskDelay(5); //5
  }
}

void TaskDisplay(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.
  u8x8.setFont(u8x8_font_px437wyse700b_2x2_r);
  for (;;) // A Task shall never return or exit.
  {

    //u8x8.drawString(0, 2, "T: ");
    u8x8.print(steinhart);  
    u8x8.refreshDisplay();
    vTaskDelay(3); //1000 / portTICK_PERIOD_MS
    
  }
}

// callback for timer4 
ISR(TIMER4_COMPA_vect){
  //static BaseType_t xHigherPriorityTaskWoken;
  TCNT4 = 0; // preload timer to 0 
  extruder1.runSpeed();
  //Serial.println("aa");
  //xHigherPriorityTaskWoken = pdFALSE;
  //xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  /* 'Give' the semaphore to unblock the task. */
  //xSemaphoreGiveFromISR( xBinarySemaphore, NULL); //(signed portBASE_TYPE*)&xHigherPriorityTaskWoken 
  //xSemaphoreGive(xBinarySemaphore);
  //digitalWrite(LED_BUILTIN,HIGH);
  /* xHigherPriorityTaskWoken was initialised to pdFALSE.  It will have then
  been set to pdTRUE only if reading from or writing to a queue caused a task
  of equal or greater priority than the currently executing task to leave the
  Blocked state.  When this is the case a context switch should be performed.
  In all other cases a context switch is not necessary.
  NOTE: The syntax for forcing a context switch within an ISR varies between
  FreeRTOS ports.  The portEND_SWITCHING_ISR() macro is provided as part of
  the Cortex-M3 port layer for this purpose.  taskYIELD() must never be called
  from an ISR! */
  //portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
 }

 ISR(TIMER4_COMPB_vect){
 }



