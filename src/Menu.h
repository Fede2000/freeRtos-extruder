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
        bool has_menu = false;
        char *title = NULL;
        uint8_t currentMenu = 0; 
        int itemIdx = 0;
        int nMenuItems = 0;
        bool isSelectable = false; 
        bool isSelected = false; 
        int topSpacing;

    //Menu(void){}
    Menu(char * ptTitle = NULL){
        title = ptTitle;  // TODO: rimuovere title da Menu -> in Page

    }

    /*
    void addTitle(char * ptTitle = NULL){
        title = ptTitle;
    }
    */
    void addMenuString(char * ptItem){
        has_menu = true;

        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = NULL;
            menu_values_double[itemIdx++] = NULL;
            nMenuItems++;
        }
    }
    void addMenuStringValue(char * ptItem, void *ptValue){
        has_menu = true;

        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = ptValue;
            menu_values_double[itemIdx++] = NULL;
            nMenuItems++;
        }
    }
    /*void addMenuStringValue(char * ptItem, double *ptValue){
        if(itemIdx < MAX_MENU_ITEMS){
            menu_strings[itemIdx] = ptItem;
            menu_values_float[itemIdx] = NULL;
            menu_values_double[itemIdx++] = ptValue;
        }
    }
*/
    void updateMenu(int aUiKeyCode);
    void drawMenu();    
      
}; 

class Page : public Menu
{   
    public:     
        Page(char *aPtTitle = NULL): Menu(aPtTitle){ }
        virtual void drawPage();
        void drawTitle();  
        void drawButton(int x, int y, int id, char * name, const u8g_fntpgm_uint8_t *font);
}; 



class StatusPage : public Page
{   
    public:     
        StatusPage(char *aPtTitle = NULL): Page(aPtTitle){ nMenuItems = 2; }
        void drawPage();
}; 

extern U8GLIB_ST7920_128X64_1X u8g;
extern Page menuPage;
extern StatusPage statusPage;
extern Page setPage;
extern Page savePage;
extern Page resetPage;
extern Page * ptPages[5];

#endif