#ifndef	MENU_H
#define MENU_H

#include "U8glib.h"

class Menu 
{   
    #define MAX_MENU_ITEMS  5

    private: 
        char *menu_strings[MAX_MENU_ITEMS]; 
        float *menu_values_float[MAX_MENU_ITEMS];
        double *menu_values_double[MAX_MENU_ITEMS];   /* TODO: eliminare non usato */
        

    public: 
        char *title = NULL;
        uint8_t curruntMenu = 0; 
        int itemIdx = 0;
        bool isSelectable = true; 
        bool isSelected = false; 

    //Menu(void){}
    Menu( bool aIsSelectable = true, char * ptTitle = NULL){
        isSelectable = aIsSelectable;
        title = ptTitle;
    }
  
    void addString(char * ptItem){
        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = NULL;
            menu_values_double[itemIdx++] = NULL;
        }
    }
    void addStringValue(char * ptItem, void *ptValue){
        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = ptValue;
            menu_values_double[itemIdx++] = NULL;
        }
    }
    /*void addStringValue(char * ptItem, double *ptValue){
        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = NULL;
            menu_values_double[itemIdx++] = ptValue;
        }
    }
*/
    void updateMenu(int aUiKeyCode);

    void drawMenu(); 
    //void drawExtra(){

    //}
    
      
}; 

extern U8GLIB_ST7920_128X64_1X u8g;
extern Menu menu;
extern Menu status;
extern Menu set;
extern Menu save;
extern Menu reset;
extern Menu * ptPages[5];

#endif