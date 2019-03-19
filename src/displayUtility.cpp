#include "U8glib.h"
#include "displayUtility.h"
#include "configuration.h"

U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);

  
void Menu::drawMenu() 
{ 
    uint8_t i, h;
    u8g_uint_t w, d;
    int spacingTop = 3;
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();
    //draw title
    if(title !=NULL){
        u8g.setFont(u8g_font_7x13);
        d = (w- u8g.getStrWidth(title) )/2;
        u8g.drawStr(d, h + spacingTop, title);
        u8g.drawFrame(0,0,w, h + spacingTop*2);
        spacingTop = h + spacingTop*3;
    }

    u8g.setFont(u8g_font_6x10);
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();

    for( i = 0; i < itemIdx; i++ ) {
        d = (w-u8g.getStrWidth(menu_strings[i]))/2;
        u8g.setDefaultForegroundColor();
        if ( i == curruntMenu && isSelectable) {
            u8g.drawBox(d-2, spacingTop + i*h, w-2*d+2, h);
            u8g.setDefaultBackgroundColor();
        }
        u8g.drawStr(d, spacingTop+i*h, menu_strings[i]);
        
        if(menu_values_int[i] != NULL || menu_values_double[i]  != NULL ){
            u8g.setDefaultForegroundColor();
            u8g.setPrintPos(d -d*2 + w + 3, spacingTop + i*h+1);
            menu_values_int[i] != NULL ? u8g.print(*menu_values_int[i]) : u8g.print(*menu_values_double[i]);
        }   
    } 
} 


 