#ifndef	MENU_MANAGER_h
#define MENU_MANAGER_h

#include "configuration.h"
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include "Thread.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "displayUtility.h"  

#define KEY_NONE 0
#define KEY_NEXT 1
#define KEY_PREV -1

void drawLogo();


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class MenuManager : public Thread
{
public:
    MenuManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name,	
		 uint32_t _ticks );
    
    static ClickEncoder encoder;
    uint8_t buttonState, lastButtonState;
    Page_t page_current = Status;
    //int *ESet;
    //double *tempSetpoint;
    int uiKeyCode = 0;

    void updateMenu();
    virtual void Main() override;

private:
    
    //AccelStepper * extruder;
    //void timerIsr(); 

	uint32_t ticks;
    int tempEnc;
    bool setPageMenu = true;  

};


extern Menu menu;
extern Menu status;
extern Menu set;
extern Menu save;
extern Menu reset;

//extern MenuManager menuManager;

#endif

