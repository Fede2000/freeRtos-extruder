#ifndef	MENU_MANAGER_h
#define MENU_MANAGER_h


#include "configuration.h"
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include "Thread.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "displayUtility.h"  //Page_t



/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class MenuManager : public Thread
{
public:
    MenuManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name,	
		 uint32_t _ticks, double * aESet, double * aTempSetpoint, AccelStepper & aExtruder );
    
    uint8_t buttonState, lastButtonState;
    Page_t page_current = Status;
    double *ESet, *tempSetpoint;
    int uiKeyCode = 0;

    virtual void Main() override;

private:
    ClickEncoder encoder;
    AccelStepper extruder;
    void timerIsr(); 

	uint32_t ticks;
    int tempEnc;
    bool setPageMenu = true;  

};

#endif
