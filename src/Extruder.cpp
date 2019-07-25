#include "Extruder.h"
#include "configuration.h"
#include "Arduino.h"
/*
formula: 
    periodo [seconds] = 3/10 * 1/ (RPM * MICROSTEPPINGS * GEAR_REDUCTION)
    period = 3/(10*MICROSTEPPINGS*GEAR_REDUCTION) [costant] * 1/RPM
    period
*/

Extruder::Extruder(){
    setSpeedRpm(0);
    PERIOD_COSTANT_MS = 3.0 /(10.0 *(float)MICROSTEPPINGS*(float)GEAR_REDUCTION) * 1000.0;
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0; // initialize the counter from 0
    TCCR4A |= (1<<WGM42); // enable the CTC mode
    TCCR4B |=  (1<<CS41) | (1<<CS40); // sets the control scale bits for the timer ....011 -> 64 https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
    TIMSK4 |= (1<<OCIE4A); //enable Timer4, Ch A interruptions

}

// set timer 4-A
void Extruder::setTimer(float target_period_ms){
    if(is_input_step || run_retraction){
        period_ms = period_ms * (1-(float)EXTRUSION_ACCELERATION) + target_period_ms * (float)EXTRUSION_ACCELERATION;
    }
    //else
    //    period_ms = 2.5;
    // tick period = 4*10^(-6) [s]
    // timer = int(period_ms /tick period); -->
    // timer = int(period_ms *250);  (250 = 16.000.000 / 64 / 1.000)
    // number of ticks
    timer = int(period_ms * timer_frequency_mHz);
}

void Extruder::main(){
    //target_period_ms = 2.5 /speed_rpm;
    if(steps < 0){
        boost_period = 1;  //
        steps++;
        run_retraction = true;
    }
    else if (steps > 0) //overExtrusion
    {
        boost_period = 0.5;    //double the speed
        steps --;
        run_retraction = true;
    }
    else
    {
        boost_period = 1;
        run_retraction = false;
    }
        
    target_period_ms = PERIOD_COSTANT_MS / speed_rpm * boost_period;
    TCCR4A = 0;
    TCCR4B = 0;
    TCNT4 = 0; // initialize the counter from 0
    TCCR4A |= (1<<WGM42); // enable the CTC mode
    TCCR4B |=  (1<<CS41) | (1<<CS40); // sets the control scale bits for the timer ....011 -> 64 https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
    TIMSK4 |= (1<<OCIE4A); //enable Timer4, Ch A interruptions

    setTimer( target_period_ms );
    OCR4A = timer; //sets the counter compare value
    
    //sei();
    
    /*
        If the interrupt is enabled, the interrupt handler
        routine can be used for updating the TOP value. However, changing the TOP to a value close to BOTTOM when
        the counter is running with none or a low prescaler value must be done with care since the CTC mode does not
        have the double buffering feature. If the new value written to OCRnA or ICRn is lower than the current value of
        TCNTn, the counter will miss the compare match. The counter will then have to count to its maximum value
        (0xFFFF) and wrap around starting at 0x0000 before the compare match can occur. In many cases this feature is
        not desirable. An alternative will then be to use the fast PWM mode using OCRnA for defining TOP (WGMn3:0 =
        15) since the OCRnA then will be double buffered.
    */

    /* 
        17.9.2 Clear Timer on Compare Match (CTC) Mode
        In Clear Timer on Compare or CTC mode (WGMn3:0 = 4 or 12), the OCRnA or ICRn Register are used to manipulate the counter resolution. In CTC mode the counter is cleared to zero when the counter value (TCNTn) matches
        either the OCRnA (WGMn3:0 = 4) or the ICRn (WGMn3:0 = 12). The OCRnA or ICRn define the top value for the
        counter, hence also its resolution. This mode allows greater control of the compare match output frequency. It also
        simplifies the operation of counting external events.
        The timing diagram for the CTC mode is shown in Figure 17-6. The counter value (TCNTn) increases until a compare match occurs with either OCRnA or ICRn, and then counter (TCNTn) is cleared.
    */

}
//  nSteps always > 0
bool Extruder::retract(){
    if(is_retraction_enabled){
        period_ms = 2.5;
        PORTA |= 1 << PORTA6;
        steps -= steps_to_retract;
        //boost_period = 1;
        run_retraction = true;
    }
}

bool Extruder::overExtrude(){
    period_ms = 10.0;
    PORTA &= ~(1 << PORTA6);
    if(is_retraction_enabled){
        steps += steps_to_retract;  //+ over extrude
        //boost_period = 0.5;
        //run_retraction = false;
    }
}

int Extruder::setSpeedRpm(float speed){
    
    if(speed > MAX_SETPOINT_SPEED){
        speed_rpm = MAX_SETPOINT_SPEED;
        return 0;
    }
    else if(speed <= MIN_SETPOINT_SPEED){
        speed_rpm = MIN_SETPOINT_SPEED;
        is_input_step = false;
        digitalWrite(E_ENABLE_PIN,HIGH); // free the motor
        return 0;
    }
    speed_rpm = speed;
}

// false: active, true: inactive
void Extruder::setStatus(bool status){
    digitalWrite(E_ENABLE_PIN,status);
}
void Extruder::incrementSpeed(int i){
    setSpeedRpm(speed_rpm + i);
}
void Extruder::incrementRetraction(int i){
    steps_to_retract += i;
    if(steps_to_retract < 0)
        steps_to_retract  = 0;
    
}

