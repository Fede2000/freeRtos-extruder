
#include "U8glib.h"
#include "Menu.h"

U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);
Menu menu(true, "MENU");
Menu status(false,"STATUS");
Menu set(true, "SETTINGS");
Menu save(false, "SAVE");
Menu reset(false, "RESET");

Menu *ptPages[5] = { &menu, &status, &set, &save, &reset };
  
void Menu::drawMenu() 
{ 
    uint8_t i, h;
    u8g_uint_t w, d;
    int spacingTop = 3;
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();
    //draw title
    if(title !=NULL){
        u8g.setFont(u8g_font_7x13B);
        d = (w- u8g.getStrWidth(title) )/2;
        u8g.drawStr(d, h + spacingTop, title);  //centered
        u8g.drawFrame(0,0,w, h + spacingTop*2);
        spacingTop = h + spacingTop*3;
    }

    u8g.setFont(u8g_font_profont12);     //u8g_font_6x10
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();

    for( i = 0; i < itemIdx; i++ ) {
        d = (w-u8g.getStrWidth(menu_strings[i]))/2;
        u8g.setDefaultForegroundColor();
        
        
        if ( i == curruntMenu && isSelectable) {

            /*  DRAW box  centered */
            /*
            u8g.drawBox(d-2, spacingTop + i*h, w-2*d+2, h);
            u8g.setDefaultBackgroundColor();
            */
            if (isSelected){
                u8g.setFont(u8g_font_profont15);
                u8g.setFontRefHeightText();
                u8g.setFontPosTop();
            }

            u8g.drawStr(2, spacingTop+i*h, ">");
            u8g.setFont(u8g_font_profont12);
            u8g.setFontRefHeightText();
            u8g.setFontPosTop();
        }

        //u8g.drawStr(d, spacingTop+i*h, menu_strings[i]); // centered
        u8g.drawStr(10, spacingTop+i*h, menu_strings[i]);
        
        if(menu_values_float[i] != NULL || menu_values_double[i]  != NULL ){
            u8g.setDefaultForegroundColor();
            //u8g.setPrintPos(d -d*2 + w + 3, spacingTop + i*h+1); centered
            u8g.setPrintPos(10 -d*2 + w + 3, spacingTop + i*h+1);
            menu_values_float[i] != NULL ? u8g.print(*menu_values_float[i]) : u8g.print(*menu_values_double[i]);
        }

        //drawExtra();   
    } 
} 