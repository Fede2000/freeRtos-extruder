#include "temperatureManager.h"
#include "configuration.h"
#include <PID_v1.h>
#include <Arduino.h>

TemperatureManager::TemperatureManager(unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name, uint32_t _ticks ) : 
                                                                                    myPID(&temperature, &output, &tempSetpoint, (double) DEFAULT_Kp, (double) DEFAULT_Ki, (double) DEFAULT_Kd, (int) 0 /*PID::DIRECT*/) , 
                                                                                    Thread{ _stackDepth, _priority, _name },
                                                                                    ticks{ _ticks }
{
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(0,PID_MAX);
    tempSetpoint = 35; // derault value
    alpha = 0.50;
    HEATER_ENABLED = false;

    //TCCR2B = TCCR2B & B11111000 | B00000111; 
    TCCR2B |=  (1<<CS41) | (1<<CS41) | (1<<CS40); // set pwm frequency (pin 10 & 9) to 30.64Hz: prescaler 1024
    getTemperature();
    setStage();
}

void TemperatureManager::setStage(){
    if((temperature-tempSetpoint)>(PREVENT_THERMAL_RUNAWAY_HYSTERESIS-1)) 
        stage = 2; //cooling
    else if((temperature-tempSetpoint)<-(PREVENT_THERMAL_RUNAWAY_HYSTERESIS-1)) 
        stage = 0;  //heating
    else
        stage = 1; //holding temperature
    Serial.println(stage);
}
void TemperatureManager::initVariables(){
    #ifdef PREVENT_THERMAL_RUNAWAY
        THERMAL_RUNAWAY_AT = millis();
        t1_temperature = temperature -5;
        t2_temperature = temperature -10;
    #endif

}

void TemperatureManager::getTemperature(){

    double average;
 
    //analogRead(THERMISTOR_PIN);                     // reading stability bugfix  https://electronics.stackexchange.com/questions/213851/arduino-analogread-neighbor-pin-noise-on-adc-even-with-big-delay
    average = (double) analogRead(THERMISTOR_PIN);  // reading stability bugfix

    average = 1023.0 / average - 1;
    average = (double) SERIESRESISTOR / average;
   
    // steinhart formula

    average = average / (double) THERMISTORNOMINAL;         // (R/Ro)
    average = log(average);                                 // ln(R/Ro)
    average /= B_COEFFICIENT;                               // 1/B * ln(R/Ro)
    average += 1.0 / (TEMPERATURENOMINAL + 273.15);         // + (1/To)
    average = 1.0 / average;                                // Invert
    average -= 273.15;                                      // convert to C
    temperature = average*alpha + temperature*(1-alpha);   //FIR filter

}
double TemperatureManager::readTemperature(){
    return temperature;
}

void TemperatureManager::Main() {
    #ifdef PREVENT_THERMAL_RUNAWAY        
        PREVENT_THERMAL_RUNAWAY_IS_ACTIVE = true;
        THERMAL_RUNAWAY_AT = millis();
        t1_temperature = temperature -5;
        t2_temperature = t1_temperature - 10;
    #endif //PREVENT_THERMAL_RUNAWAY

    #ifdef PREVENT_COLD_EXTRUSION
        COLD_EXTRUSION_FLAG = false; // TODO: true or false
        PREVENT_COLD_EXTRUSION_IS_ACTIVE = true;
    #endif //PREVENT_COLD_EXTRUSION

    for (;;)
    {
        getTemperature();
        

        #ifdef PREVENT_COLD_EXTRUSION
        if( PREVENT_COLD_EXTRUSION_IS_ACTIVE)
            COLD_EXTRUSION_FLAG = temperature > (tempSetpoint - PREVENT_COLD_EXTRUSION_DELTA_TEMP) ? false : true;
        else  
            COLD_EXTRUSION_FLAG = false;
        #endif
        
        #ifdef PREVENT_THERMAL_RUNAWAY
        if(stage==0 && (temperature-tempSetpoint>=0))
            stage=1;
        else if(stage==2 && (temperature-tempSetpoint<=0))
            stage=1;
        if( PREVENT_THERMAL_RUNAWAY_IS_ACTIVE && HEATER_ENABLED ){
            if( temperature > MAX_TEMPERATURE || temperature < MIN_TEMPERATURE)
                THERMAL_RUNAWAY_FLAG = true;

            else if( (millis() - THERMAL_RUNAWAY_AT) > (THERMAL_RUNAWAY_PERIOD + extraTime)){
                extraTime = 0;
                switch (stage)
                {
                case 0:  //heating
                    if((temperature-t2_temperature <  (float)WATCH_TEMP_INCREASE)  &&  (temperature-t1_temperature)< (t1_temperature-t2_temperature)) 
                        THERMAL_RUNAWAY_FLAG = true;
                    break;
                case 1:  //holding temperature
                    if(abs(tempSetpoint - temperature) > PREVENT_THERMAL_RUNAWAY_HYSTERESIS)
                        THERMAL_RUNAWAY_FLAG = true;
                    break;
                case 2:  //cooling 
                    if(((temperature - t1_temperature) > 2) && abs(tempSetpoint - temperature) > 5)   //if it's heating instead of cooling down & temp is distant from temp setpoint
                        THERMAL_RUNAWAY_FLAG = true;
                    break;
                default:
                    break;
                }
                THERMAL_RUNAWAY_AT = millis();
                t2_temperature = t1_temperature;
                t1_temperature = temperature;
            }
        }
        //else 
            //THERMAL_RUNAWAY_FLAG=false;
        #endif
        EXTRUDER_SHOULD_RUN = !THERMAL_RUNAWAY_FLAG && !COLD_EXTRUSION_FLAG;
        
        if(!THERMAL_RUNAWAY_FLAG && HEATER_ENABLED){
            //myPID.Compute();
            output = get_pid_output();
            
            analogWrite(HEATER_PIN, output); // output);
        }
        else             
            analogWrite(HEATER_PIN, 0);
             
        
        vTaskDelay(ticks);  // one tick delay (15ms) in between reads for stability
    }
}

void TemperatureManager::setTemperature( double temperatureSetpoint){
    #ifdef PREVENT_THERMAL_RUNAWAY
        extraTime = 15000;
    #endif
    tempSetpoint = temperatureSetpoint;
     if(abs(tempSetpoint) > 100)
        myPID.SetTunings(DEFAULT_Kp,DEFAULT_Ki,DEFAULT_Kd);
    else
    {
        myPID.SetTunings(DEFAULT_Kp/1.5,DEFAULT_Ki/1.5,DEFAULT_Kd/1.5);
    }
    if(temperatureSetpoint > MAX_SETPOINT_TEMP)
        tempSetpoint = MAX_SETPOINT_TEMP;
    else if(temperatureSetpoint < MIN_SETPOINT_TEMP)
        tempSetpoint = MIN_SETPOINT_TEMP;
}
void TemperatureManager::switchMode(){
    if(HEATER_ENABLED){
        HEATER_ENABLED= false;
        myPID.SetMode(MANUAL);
    }
    else
    {
        #ifdef PREVENT_THERMAL_RUNAWAY
            extraTime = 15000;
        #endif
        HEATER_ENABLED= true;
        myPID.SetMode(AUTOMATIC);
    }
    
}

void TemperatureManager::incrementTemperature( int i ){
    setTemperature( tempSetpoint + i);
}


float TemperatureManager::get_pid_output() {
    float pid_output;
    static hotend_pid_t work_pid[HOTENDS];
    static float temp_iState[HOTENDS] = { 0 },
                temp_dState[HOTENDS] = { 0 };
    static bool pid_reset[HOTENDS] = { false };
    float pid_error = tempSetpoint - temperature;
    work_pid[HOTEND_INDEX].Kd = PID_K2 * DEFAULT_Kd * (temperature - temp_dState[HOTEND_INDEX]) + float(PID_K1) * work_pid[HOTEND_INDEX].Kd;
    temp_dState[HOTEND_INDEX] = temperature;

    if (tempSetpoint == 0
    || pid_error < -(PID_FUNCTIONAL_RANGE)) {
        pid_output = 0;
        pid_reset[HOTEND_INDEX] = true;
        }
        else if (pid_error > PID_FUNCTIONAL_RANGE) {
        pid_output = BANG_MAX;
        pid_reset[HOTEND_INDEX] = true;
    }
    else {
        if (pid_reset[HOTEND_INDEX]) {
            temp_iState[HOTEND_INDEX] = 0.0;
            pid_reset[HOTEND_INDEX] = false;
        }
        temp_iState[HOTEND_INDEX] += pid_error;
        work_pid[HOTEND_INDEX].Kp = DEFAULT_Kp * pid_error;
        work_pid[HOTEND_INDEX].Ki = DEFAULT_Ki * temp_iState[HOTEND_INDEX];

        pid_output = work_pid[HOTEND_INDEX].Kp + work_pid[HOTEND_INDEX].Ki - work_pid[HOTEND_INDEX].Kd;

        if (pid_output > PID_MAX) {
            if (pid_error > 0) temp_iState[HOTEND_INDEX] -= pid_error; // conditional un-integration
            pid_output = PID_MAX;
        }
        else if (pid_output < 0) {
            if (pid_error < 0) temp_iState[HOTEND_INDEX] -= pid_error; // conditional un-integration
            pid_output = 0;
        }
    }

  return pid_output;
}