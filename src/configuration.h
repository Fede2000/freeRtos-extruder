#ifndef	CONFIGURATION_h
#define CONFIGURATION_h

#define ENCODER_PIN1 31
#define ENCODER_PIN2 33
#define ENCODER_BTN 35
 
#define E_STEP_PIN 26
#define E_DIR_PIN 28
#define E_ENABLE_PIN 24

#define THERMISTOR_PIN 13
#define HEATER_PIN 10
#define BUZZ_PIN 33 

#define EN_PIN  57    // enable pin
#define EN_M_PIN  41 // manually enable pin "KILL PIN"



/**
 
 * thermistor and temperature settings
 
 **/

#define PREVENT_THERMAL_RUNAWAY
#define PREVENT_COLD_EXTRUSION
#define EXTRUDE_MINTEMP 20 //TODO: 150
#define DEFAULT_TEMP 35
#define DEFAULT_SPEED 60 //RPM 

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

#define CONST_KP 24
#define CONST_KI 0.5
#define CONST_KD 0.3


//EEPROM  TODO: almost all useless -> remove their use
#define ADDRESS_TEMPERATURE  0
#define ADDRESS_SPEED  1
#define ADDRESS_CK  2
#define EEPROM_CK_VALUE 12

#endif


