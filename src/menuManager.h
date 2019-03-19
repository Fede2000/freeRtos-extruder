#ifndef	MENU_MANAGER_h
#define MENU_MANAGER_h

#include "configuration.h"
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include "Thread.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "displayUtility.h"  //Page_t

extern Menu menu;
extern Menu status;
extern Menu set;
extern Menu save;
extern Menu reset;

extern U8GLIB_ST7920_128X64_1X u8g;


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class MenuManager : public Thread
{
public:
    MenuManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name,	
		 uint32_t _ticks, int * aESet, double * aTempSetpoint, AccelStepper * aExtruder );
    
    static ClickEncoder encoder;
    uint8_t buttonState, lastButtonState;
    Page_t page_current = Status;
    int *ESet;
    double *tempSetpoint;
    int uiKeyCode = 0;

    virtual void Main() override;

private:
    
    AccelStepper * extruder;
    //void timerIsr(); 

	uint32_t ticks;
    int tempEnc;
    bool setPageMenu = true;  

};

#endif

