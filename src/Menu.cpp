
#include "U8glib.h"
#include "Menu.h"
#include "Arduino.h"

U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);
Page menuPage("MENU");
Page statusPage("STATUS");
Page setPage("SETTINGS");
Page savePage("SAVE");
Page resetPage("RESET");

Page *ptPages[5] = { &menuPage, &statusPage, &setPage, &savePage, &resetPage };
  
void Menu::drawMenu() 
{ 

    uint8_t i, h;
    u8g_uint_t w, d;

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
            u8g.drawBox(d-2, topSpacing + i*h, w-2*d+2, h);
            u8g.setDefaultBackgroundColor();
            */
            if (isSelected){
                u8g.setFont(u8g_font_profont15);
                u8g.setFontRefHeightText();
                u8g.setFontPosTop();
            }

            u8g.drawStr(2, topSpacing+i*h, ">");
            u8g.setFont(u8g_font_profont12);
            u8g.setFontRefHeightText();
            u8g.setFontPosTop();
        }

        //u8g.drawStr(d, topSpacing+i*h, menu_strings[i]); // centered
        u8g.drawStr(10, topSpacing+i*h, menu_strings[i]);
        
        if(menu_values_float[i] != NULL || menu_values_double[i]  != NULL ){
            u8g.setDefaultForegroundColor();
            //u8g.setPrintPos(d -d*2 + w + 3, topSpacing + i*h+1); centered
            u8g.setPrintPos(10 -d*2 + w + 3, topSpacing + i*h+1);
            menu_values_float[i] != NULL ? u8g.print(*menu_values_float[i]) : u8g.print(*menu_values_double[i]);
        }

        //drawExtra();   
    } 
} 


void Page::drawPage(){
    topSpacing = 3;
    
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();
    
}

void Page::drawTitle(){
    uint8_t i, h;
    u8g_uint_t w, d;

    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();
    //draw title
    u8g.setFont(u8g_font_7x13B);
    d = (w- u8g.getStrWidth(title) )/2;
    u8g.drawStr(d, h + topSpacing, title);  //centered
    u8g.drawFrame(0,0,w, h + topSpacing*2);
    topSpacing = h + topSpacing*3;
}