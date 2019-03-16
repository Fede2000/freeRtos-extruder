#ifndef	THREAD_h
#define THREAD_h

#include <Arduino_FreeRTOS.h>

class Thread
{
    public:
        Thread( unsigned portSHORT _stackDepth, UBaseType_t _priority, const char* _name = "" )
        {
            xTaskCreate( task, _name, _stackDepth, this, _priority, &this->taskHandle );
        }

        virtual void Main() = 0;

    protected:
        static void task( void* _params )
        {
            Thread* p = static_cast<Thread*>( _params );
            p->Main();
        }

        TaskHandle_t taskHandle;
};

#endif