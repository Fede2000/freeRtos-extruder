#ifndef	TEMPERATURE_MANAGER_h
#define TEMPERATURE_MANAGER_h


#include "configuration.h"
#include <PID_v1.h>
#include <Arduino_FreeRTOS.h>
#include "Thread.h"
#include "Arduino.h"
#include "./thermistor/thermistor.h"



#if HOTENDS <= 1
  #define HOTEND_INDEX  0
  #define E_UNUSED() UNUSED(e)
#else
  #define HOTEND_INDEX  e
  #define E_UNUSED()
#endif

#define PID_K2 (1-float(PID_K1))
// PID storage
typedef struct { float Kp, Ki, Kd;     } PID_t;
typedef PID_t hotend_pid_t;

/*
enum ADCSensorState : char {
  StartSampling,
  #ifdef THERMISTOR_PIN //HAS_TEMP_ADC_0
    PrepareTemp_0,
    MeasureTemp_0,
  #endif
  #if HAS_HEATED_BED
    PrepareTemp_BED,
    MeasureTemp_BED,
  #endif
  #if HAS_TEMP_CHAMBER
    PrepareTemp_CHAMBER,
    MeasureTemp_CHAMBER,
  #endif
  #if HAS_TEMP_ADC_1
    PrepareTemp_1,
    MeasureTemp_1,
  #endif
  #if HAS_TEMP_ADC_2
    PrepareTemp_2,
    MeasureTemp_2,
  #endif
  #if HAS_TEMP_ADC_3
    PrepareTemp_3,
    MeasureTemp_3,
  #endif
  #if HAS_TEMP_ADC_4
    PrepareTemp_4,
    MeasureTemp_4,
  #endif
  #if HAS_TEMP_ADC_5
    PrepareTemp_5,
    MeasureTemp_5,
  #endif
  #if ENABLED(FILAMENT_WIDTH_SENSOR)
    Prepare_FILWIDTH,
    Measure_FILWIDTH,
  #endif
  #if HAS_ADC_BUTTONS
    Prepare_ADC_KEY,
    Measure_ADC_KEY,
  #endif
  SensorsReady, // Temperatures ready. Delay the next round of readings to let ADC pins settle.
  StartupDelay  // Startup, delay initial temp reading a tiny bit so the hardware can settle
};
*/

// A temperature sensor
typedef struct TempInfo {
  uint16_t acc;
  int16_t raw;
  float current;
} temp_info_t;

// A PWM heater with temperature sensor
typedef struct HeaterInfo : public TempInfo {
  int16_t target;
  uint8_t soft_pwm_amount;
} heater_info_t;

// A heater with PID stabilization
template<typename T>
struct PIDHeaterInfo : public HeaterInfo {
  T pid;  // Initialized by settings.load()
};

#ifdef PIDTEMP
  typedef struct PIDHeaterInfo<hotend_pid_t> hotend_info_t;
#else
  typedef heater_info_t hotend_info_t;
#endif


/**
 * States for ADC reading in the ISR
 */
enum ADCSensorState : char {
  StartSampling,
  PrepareTemp_0,
  MeasureTemp_0,

  SensorsReady, // Temperatures ready. Delay the next round of readings to let ADC pins settle.
  StartupDelay  // Startup, delay initial temp reading a tiny bit so the hardware can settle
};


// Temperature sensor read value ranges
typedef struct { int16_t raw_min, raw_max; } raw_range_t;
typedef struct { int16_t mintemp, maxtemp; } celsius_range_t;
typedef struct { int16_t raw_min, raw_max, mintemp, maxtemp; } temp_range_t;

#define THERMISTOR_ADC_RESOLUTION       1024           // 10-bit ADC .. shame to waste 12-bits of resolution on 32-bit
#define THERMISTOR_ABS_ZERO_C           -273.15f       // bbbbrrrrr cold !
#define THERMISTOR_RESISTANCE_NOMINAL_C 25.0f          // mmmmm comfortable


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class TemperatureManager : public Thread
{

private:
    
	uint32_t ticks;
    PID myPID;
    float get_pid_output();
    float analog_to_celsius_hotend(const int raw, const uint8_t e);
    void updateTemperaturesFromRawValues();
    void readings_ready();
    void set_current_temp_raw();
    void isr();
    int8_t temp_count = -1;
    static volatile bool temp_meas_ready;
    void allarm();

    #ifdef PREVENT_THERMAL_RUNAWAY
        unsigned long THERMAL_RUNAWAY_AT = millis(), extraTime = 15000;
        double t1_temperature, t2_temperature;
    #endif

public:
    double output;
    double temperature;
    double tempSetpoint;

    static hotend_info_t temp_hotend[HOTENDS];

    float alpha;
    bool HEATER_ENABLED, PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = false, PREVENT_COLD_EXTRUSION_IS_ACTIVE = false, EXTRUDER_SHOULD_RUN,
    THERMAL_RUNAWAY_FLAG = false, COLD_EXTRUSION_FLAG=false;
    
    int stage = 0;  //HEATING_PHASE = 0, HOLDING_PHASE = 1, COOLING_PHASE = 2; 
    

    TemperatureManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks );

    void getTemperature_deprecated();
    void setTemperature(double temperatureSetpoint);
    void incrementTemperature(int i);
    void switchMode();
    void setStage(); void initVariables();
    double readTemperature();
    void Main();


};

//extern TemperatureManager temperatureManager;

#endif

