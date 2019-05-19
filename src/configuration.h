#ifndef	CONFIGURATION_h
#define CONFIGURATION_h

/**
 
 * pin settings
 
 **/
#define ENCODER_PIN1 31
#define ENCODER_PIN2 33
#define ENCODER_BTN 35
 
#define E_STEP_PIN 26
#define E_DIR_PIN 28
#define E_ENABLE_PIN 24

#define THERMISTOR_PIN 13
#define HEATER_PIN 10
#define BUZZ_PIN 37 

#define EN_PIN  57    // enable pin
#define EN_M_PIN  41 // manually enable pin "KILL PIN"



/**
 
 * thermistor and temperature settings
 
 **/

#define PREVENT_THERMAL_RUNAWAY                 //comment out to disable
#define PREVENT_THERMAL_RUNAWAY_THRESHOLD 35
#define PREVENT_THERMAL_RUNAWAY_TIME 10000      //ms
#define PREVENT_COLD_EXTRUSION                  //comment out to disable
#define PREVENT_COLD_EXTRUSION_DELTA_TEMP 5    

#define DEFAULT_TEMP 35
#define DEFAULT_SPEED 60    //RPM 
#define MAX_SET_TEMP 300
#define MAX_SET_SPEED 300


#define THERMISTORNOMINAL 100000      
#define TEMPERATURENOMINAL 25       // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5                // how many samples to take and average, more takes longer, but is smoother
#define B_COEFFICIENT 3950          // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 4700         // the value of the RAMPS resistor

//  PID settings
#define CONST_KP 10         //kp
#define CONST_KI 0.5        //ki
#define CONST_KD 0.3        //kd


/**
 
 * EEPROM settings
 
 **/

//  EEPROM  
//TODO: almost all useless -> remove their use
#define ADDRESS_TEMPERATURE  0
#define ADDRESS_SPEED  1
#define ADDRESS_CK  2
#define EEPROM_CK_VALUE 12

/** 
 * varie
**/

#define MICROSTEPPINGS 4 // default 4
#define GEAR_REDUCTION 3.75 

//#define DRAW_LOGO

#endif


