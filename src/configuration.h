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

#define EXTRUDER_EN_PIN  57    // enable pin
#define EXTRUDER_BTN_EN_PIN  41 // manually enable pin "KILL PIN"



//===========================================================================
//======================== Thermal Runaway Protection =======================
//===========================================================================

/**
 * Thermal Protection provides additional protection to your printer from damage
 * and fire. Marlin always includes safe min and max temperature ranges which
 * protect against a broken or disconnected thermistor wire.
 *
 * The issue: If a thermistor falls out, it will report the much lower
 * temperature of the air in the room, and the the firmware will keep
 * the heater on.
 *
 * If you get "Thermal Runaway" or "Heating failed" errors the
 * details can be tuned in Configuration_adv.h
 */
#define PREVENT_THERMAL_RUNAWAY                 //comment out to disable
#define PREVENT_THERMAL_RUNAWAY_HYSTERESIS 6    //^C
#define THERMAL_RUNAWAY_PERIOD 4000            //ms
#define WATCH_TEMP_INCREASE 0.5                   //^C
#define PREVENT_COLD_EXTRUSION                  //comment out to disable
#define PREVENT_COLD_EXTRUSION_DELTA_TEMP 5     //^C

//===========================================================================
//==================== thermistor and temperature settings ==================
//===========================================================================

#define DEFAULT_TEMP 35
#define DEFAULT_SPEED 60    //RPM 
#define MAX_SETPOINT_TEMP 300
#define MIN_SETPOINT_TEMP 0
#define MAX_SETPOINT_SPEED 300
#define MIN_SETPOINT_SPEED 0

#define DEFAULT_RETRACTION_STEPS 500
#define EXTRUSION_ACCELERATION  0.05 // higher accelerations=> lees torque; slower accelerations => more torque, but less reactivity.  range[0.02 - 0.15]

#define THERMISTORNOMINAL 100000      
#define TEMPERATURENOMINAL 25       // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5                // how many samples to take and average, more takes longer, but is smoother
#define B_COEFFICIENT 3950          // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 4700         // the value of the RAMPS resistor

// This defines the number of extruders
// :[1, 2, 3, 4, 5, 6]
#define EXTRUDERS 1
#ifndef HOTENDS
  #define HOTENDS EXTRUDERS
#endif

//===========================================================================
//============================= PID Settings ================================
//===========================================================================
// PID Tuning Guide here: http://reprap.org/wiki/PID_Tuning
// http://forum.seemecnc.com/download/file.php?id=14309&sid=16e4a26fb0f27e18489562c0d0d18d3d
/*
if it overshoots a lot and oscillates, either the integral gain needs to be increased or all gains should be reduced
Too much overshoot? Increase D, decrease P.
Response too damped? Increase P.
Ramps up quickly to a value below target temperature (0-160 fast) and then slows down as it approaches target (160-170 slow, 170-180 really slow, etc) temperature? Try increasing the I constant.
*/

/*
#define DEFAULT_Kp 22.2
#define DEFAULT_Ki 1.08
#define DEFAULT_Kd 114
*/
#define DEFAULT_Kp 10.2
#define DEFAULT_Ki 0.09
#define DEFAULT_Kd 195

#define PID_K1 0.85      // Smoothing factor within any PID loop default .95

#define PID_FUNCTIONAL_RANGE 10 // If the temperature difference between the target temperature and the actual temperature
                                  // is more than PID_FUNCTIONAL_RANGE then the PID will be shut off and the heater will be set to min/max
                                  
#define BANG_MAX 200     // Limits current to nozzle while in bang-bang mode; 255=full current
#define PID_MAX BANG_MAX // Limits current to nozzle while PID is active (see PID_FUNCTIONAL_RANGE below); 255=full current

/*
#define DEFAULT_Kp 10.5         //kp  15: oscillation period 23s -> kd =1/3*23, kp 15->*0.7
#define DEFAULT_Ki 0.1        //ki
#define DEFAULT_Kd 3       //kd
#define PID_MAX  200
*/


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
#define debug
#define MICROSTEPPINGS 4 // default 4
#define GEAR_REDUCTION 3.75 

//stop btn function definition
//#define STOP_BTN_TEST     //stop btn usato x spurgare
#define STOP_BTN_CONTROLL   //stop btn usato x comandare estrusore
#define DRAW_LOGO

#endif


