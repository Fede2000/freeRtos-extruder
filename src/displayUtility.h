#ifndef	DISPLAYUTILITY_h
#define DISPLAYUTILITY_h
#include "U8glib.h"

class Menu 
{   
    #define MAX_MENU_ITEMS  5

    private: 
    U8GLIB_ST7920_128X64_1X * ptU8g;
    char *menu_strings[MAX_MENU_ITEMS]; 
    

    public: 
    uint8_t curruntMenu = 0; 
    char itemIdx = 0;

    

    //Menu(void){}
    Menu(U8GLIB_ST7920_128X64_1X * ptPage){
        ptU8g = ptPage;
    }
    void addItem(char * ptItem){
        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx++] = ptItem;
        }
    }

    void updateMenu(int aUiKeyCode);

    void drawMenu(); 
      
    /*void printid() 
    { 
        cout << "Geek id is: " << id; 
    } */
}; 






#endif