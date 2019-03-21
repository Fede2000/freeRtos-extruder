#ifndef	MENU_MANAGER_h
#define MENU_MANAGER_h

#include "configuration.h"
#include <Arduino_FreeRTOS.h>
#include <AccelStepper.h>
#include "Thread.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "displayUtility.h"  
#include "temperatureManager.h"

#define KEY_NONE 0
#define KEY_NEXT 1
#define KEY_PREV -1

typedef enum {
    Menu_p,
    Status,
    Settings,
    Save,
    Reset
}Page_t;  // TODO: DELETE


void drawLogo();


/* https://drive.google.com/file/d/1SBhXfaA_kXBOX_d44FqEMjZ5Gr1imTRZ/view */
class MenuManager : public Thread
{
public:
    MenuManager( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name,	
		 uint32_t _ticks, TemperatureManager *  aTemperatureManager );
    
    static ClickEncoder encoder;
    uint8_t buttonState, lastButtonState;
    Page_t page_current = Status;
    int uiKeyCode = 0;

    void updateMenu();
    void Main();

private:


	uint32_t ticks;
    int tempEnc;
    bool setPageMenu = true;  
    TemperatureManager * temperatureManagerTest;

};


//extern MenuManager menuManager;

#endif
