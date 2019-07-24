#ifndef	CONFIGURATION_h
#define CONFIGURATION_h

#define VERSION "3.6.3"
#define DEBUG   

//===========================================================================
//=============================== Pin Settings ==============================
//===========================================================================
#define ENCODER_PIN1 31
#define ENCODER_PIN2 33
#define ENCODER_BTN 35
 
#define E_STEP_PIN 26
#define E_DIR_PIN 28
#define E_ENABLE_PIN 24


#define THERMISTOR_PIN 13     //analog
#define HEATER_PIN 10
#define BUZZ_PIN 37 

#define EXTRUDER_EN_PIN  57    // enable pin
#define EXTRUDER_BTN_EN_PIN  41 // manually enable pin "KILL PIN"


//===========================================================================
//============================= Thermal Settings ============================
//===========================================================================

/**
 * --NORMAL IS 4.7kohm PULLUP!-- 1kohm pullup can be used on hotend sensor, using correct resistor and table
 *
 * Temperature sensors available:
 *
 *    -4 : thermocouple with AD8495
 *    -3 : thermocouple with MAX31855 (only for sensor 0)
 *    -2 : thermocouple with MAX6675 (only for sensor 0)
 *    -1 : thermocouple with AD595
 *     0 : not used
 *     1 : 100k thermistor - best choice for EPCOS 100k (4.7k pullup)
 *     2 : 200k thermistor - ATC Semitec 204GT-2 (4.7k pullup)
 *     3 : Mendel-parts thermistor (4.7k pullup)
 *     4 : 10k thermistor !! do not use it for a hotend. It gives bad resolution at high temp. !!
 *     5 : 100K thermistor - ATC Semitec 104GT-2/104NT-4-R025H42G (Used in ParCan & J-Head) (4.7k pullup)
 *   501 : 100K Zonestar (Tronxy X3A) Thermistor
 *     6 : 100k EPCOS - Not as accurate as table 1 (created using a fluke thermocouple) (4.7k pullup)
 *     7 : 100k Honeywell thermistor 135-104LAG-J01 (4.7k pullup)
 *    71 : 100k Honeywell thermistor 135-104LAF-J01 (4.7k pullup)
 *     8 : 100k 0603 SMD Vishay NTCS0603E3104FXT (4.7k pullup)
 *     9 : 100k GE Sensing AL03006-58.2K-97-G1 (4.7k pullup)
 *    10 : 100k RS thermistor 198-961 (4.7k pullup)
 *    11 : 100k beta 3950 1% thermistor (4.7k pullup)
 *    12 : 100k 0603 SMD Vishay NTCS0603E3104FXT (4.7k pullup) (calibrated for Makibox hot bed)
 *    13 : 100k Hisens 3950  1% up to 300°C for hotend "Simple ONE " & "Hotend "All In ONE"
 *    15 : 100k thermistor calibration for JGAurora A5 hotend
 *    18 : ATC Semitec 204GT-2 (4.7k pullup) Dagoma.Fr - MKS_Base_DKU001327
 *    20 : the PT100 circuit found in the Ultimainboard V2.x
 *    60 : 100k Maker's Tool Works Kapton Bed Thermistor beta=3950
 *    61 : 100k Formbot / Vivedino 3950 350C thermistor 4.7k pullup
 *    66 : 4.7M High Temperature thermistor from Dyze Design
 *    67 : 450C thermistor from SliceEngineering
 *    70 : the 100K thermistor found in the bq Hephestos 2
 *    75 : 100k Generic Silicon Heat Pad with NTC 100K MGB18-104F39050L32 thermistor
 *
 *       1k ohm pullup tables - This is atypical, and requires changing out the 4.7k pullup for 1k.
 *                              (but gives greater accuracy and more stable PID)
 *    51 : 100k thermistor - EPCOS (1k pullup)
 *    52 : 200k thermistor - ATC Semitec 204GT-2 (1k pullup)
 *    55 : 100k thermistor - ATC Semitec 104GT-2 (Used in ParCan & J-Head) (1k pullup)
 *
 *  1047 : Pt1000 with 4k7 pullup
 *  1010 : Pt1000 with 1k pullup (non standard)
 *   147 : Pt100 with 4k7 pullup
 *   110 : Pt100 with 1k pullup (non standard)
 *
 *  1000 : Custom - Specify parameters in Configuration_adv.h
 *
 *         Use these for Testing or Development purposes. NEVER for production machine.
 *   998 : Dummy Table that ALWAYS reads 25°C or the temperature defined below.
 *   999 : Dummy Table that ALWAYS reads 100°C or the temperature defined below.
 *
 * :{ '0':"Not used", '1':"100k / 4.7k - EPCOS", '2':"200k / 4.7k - ATC Semitec 204GT-2", '3':"Mendel-parts / 4.7k", '4':"10k !! do not use for a hotend. Bad resolution at high temp. !!", '5':"100K / 4.7k - ATC Semitec 104GT-2 (Used in ParCan & J-Head)", '501':"100K Zonestar (Tronxy X3A)", '6':"100k / 4.7k EPCOS - Not as accurate as Table 1", '7':"100k / 4.7k Honeywell 135-104LAG-J01", '8':"100k / 4.7k 0603 SMD Vishay NTCS0603E3104FXT", '9':"100k / 4.7k GE Sensing AL03006-58.2K-97-G1", '10':"100k / 4.7k RS 198-961", '11':"100k / 4.7k beta 3950 1%", '12':"100k / 4.7k 0603 SMD Vishay NTCS0603E3104FXT (calibrated for Makibox hot bed)", '13':"100k Hisens 3950  1% up to 300°C for hotend 'Simple ONE ' & hotend 'All In ONE'", '18':"ATC Semitec 204GT-2 (4.7k pullup) Dagoma.Fr - MKS_Base_DKU001327" '20':"PT100 (Ultimainboard V2.x)", '51':"100k / 1k - EPCOS", '52':"200k / 1k - ATC Semitec 204GT-2", '55':"100k / 1k - ATC Semitec 104GT-2 (Used in ParCan & J-Head)", '60':"100k Maker's Tool Works Kapton Bed Thermistor beta=3950", '61':"100k Formbot / Vivedino 3950 350C thermistor 4.7k pullup", '66':"Dyze Design 4.7M High Temperature thermistor", '67':"Slice Engineering 450C High Temperature thermistor", '70':"the 100K thermistor found in the bq Hephestos 2", '71':"100k / 4.7k Honeywell 135-104LAF-J01", '147':"Pt100 / 4.7k", '1047':"Pt1000 / 4.7k", '110':"Pt100 / 1k (non-standard)", '1010':"Pt1000 / 1k (non standard)", '-4':"Thermocouple + AD8495", '-3':"Thermocouple + MAX31855 (only for sensor 0)", '-2':"Thermocouple + MAX6675 (only for sensor 0)", '-1':"Thermocouple + AD595", '998':"Dummy 1", '999':"Dummy 2", '1000':"Custom thermistor params" }
 */
#define TEMP_SENSOR_0 1
#define TEMP_SENSOR_1 0   //not implemented in code yet
#define TEMP_SENSOR_2 0   //not implemented in code yet

#if TEMP_SENSOR_0 > 0
  #define THERMISTOR_HEATER_0 TEMP_SENSOR_0
  #define HEATER_0_USES_THERMISTOR
  #define HOTEND_USES_THERMISTOR
  #if TEMP_SENSOR_0 == 1000
    #define HEATER_0_USER_THERMISTOR
  #endif
#endif

// Below this temperature the heater will be switched off
// because it probably indicates a broken thermistor wire.
#define HEATER_0_MINTEMP   5
#define HEATER_1_MINTEMP   5
#define HEATER_2_MINTEMP   5

// Above this temperature the heater will be switched off.
// This can protect components from overheating, but NOT from shorts and failures.
// (Use MINTEMP for thermistor short/failure protection.)
#define HEATER_0_MAXTEMP 275
#define HEATER_1_MAXTEMP 275
#define HEATER_2_MAXTEMP 275

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
//=========================== Security Settings =============================
//===========================================================================

/**
 * Thermal Protection provides additional protection to your printer from damage
 * and fire. Direct3D always includes safe min and max temperature ranges which
 * protect against a broken or disconnected thermistor wire.
 *
 * The issue: If a thermistor falls out, it will report the much lower
 * temperature of the air in the room, and the the firmware will keep
 * the heater on.
 */
#define PREVENT_THERMAL_RUNAWAY                 //comment out to disable
#define PREVENT_THERMAL_RUNAWAY_HYSTERESIS 8    //^C  to define the safe operating range of the hotend when in temperature.
#define THERMAL_RUNAWAY_PERIOD 4000            //ms   
#define WATCH_TEMP_INCREASE 0.35                   //^C .35 altrimenti ptr
#define PREVENT_COLD_EXTRUSION                  //comment out to disable
#define PREVENT_COLD_EXTRUSION_DELTA_TEMP 5     //^C

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
#define PIDTEMP
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



//===========================================================================
//=========================== General Settings ==============================
//===========================================================================

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
 * 
**/
#define MICROSTEPPINGS 4 // default 4
#define GEAR_REDUCTION 3.75 


#define DEFAULT_TEMP 35
#define DEFAULT_SPEED 60    //RPM 
#define MAX_SETPOINT_SPEED 300
#define MIN_SETPOINT_SPEED 0
#define MAX_SETPOINT_TEMP HEATER_0_MAXTEMP
#define MIN_SETPOINT_TEMP HEATER_0_MINTEMP

#define DEFAULT_RETRACTION_STEPS 500
#define EXTRUSION_ACCELERATION  0.05 // higher accelerations=> lees torque; slower accelerations => more torque, but less reactivity.  range[0.02 - 0.15]


//stop btn function definition
//#define STOP_BTN_TEST     //stop btn usato x spurgare
#define STOP_BTN_CONTROLL   //stop btn usato x comandare estrusore
#define DRAW_LOGO
#define ALLARM
#endif


