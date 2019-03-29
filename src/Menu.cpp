
#include "U8glib.h"
#include "Menu.h"
#include "Arduino.h"

U8GLIB_ST7920_128X64_1X u8g(23, 17, 16);
  
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
        
        
        if ( i == currentMenu && isSelectable) {

            /*  DRAW box  centered */
            /*
            u8g.drawBox(d-2, topSpacing + i*h, w-2*d+2, h);
            u8g.setDefaultBackgroundColor();
            */
            if (isSelected){
                u8g.setFont(u8g_font_profont15);  //u8g_font_6x12_75r
                u8g.setFontRefHeightText();
                u8g.setFontPosTop();
                u8g.drawStr(0, topSpacing+i*h, "<");
                //u8g.setPrintPos(2,topSpacing+i*h);
                //u8g.print(0x20);    // https://github.com/olikraus/u8glib/wiki/fontgroupx11
                u8g.setFont(u8g_font_profont12);
                u8g.setFontRefHeightText();
                u8g.setFontPosTop();
            }
            else
                u8g.drawStr(2, topSpacing+i*h, ">");
        }

        //u8g.drawStr(d, topSpacing+i*h, menu_strings[i]); // centered
        u8g.drawStr(10, topSpacing+i*h, menu_strings[i]);
        
        if(menu_values_float[i] != NULL || menu_values_double[i]  != NULL ){
            u8g.setDefaultForegroundColor();
            //u8g.setPrintPos(d -d*2 + w + 3, topSpacing + i*h+1); centered
            u8g.setPrintPos(10 -d*2 + w + 3, topSpacing + i*h+1);
            menu_values_float[i] != NULL ? u8g.print(*menu_values_float[i], 0) : u8g.print(*menu_values_double[i], 0);
        }

        //drawExtra();   
    } 
} 

void Page::drawTitle(){
    uint8_t i, h;
    u8g_uint_t w, d;
    u8g.setFont(u8g_font_7x13B);
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();    
    d = (w- u8g.getStrWidth(title) )/2;
    u8g.drawStr(d, h + topSpacing, title);  //centered
    u8g.drawRFrame(0,0,w, h + topSpacing*2, 6);
    topSpacing = h + topSpacing*3;
}

void Page::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();  
}

void Page::drawButton(int x, int y, int id, char * name = NULL , const u8g_fntpgm_uint8_t *font = u8g_font_7x13B){
    // btn1
    int h,d;
    u8g.setFont(font);
    //u8g.setFontRefHeightText();
    //u8g.setFontPosTop();
    h = u8g.getFontAscent()-u8g.getFontDescent();
    d = u8g.getStrWidth(name);
    if(id==currentMenu  ){  
        u8g.drawRBox(x -3, y -h -3, d+2*3, h+3*2, 3 );
        u8g.setDefaultBackgroundColor();
        u8g.drawStr(x, y, name);
        u8g.setDefaultForegroundColor();
    }
    else{
        u8g.drawStr(x, y, name);  
        //u8g.drawFrame(x -2, y -h -2, d+2*2, h+2*2 );
    }
    
}


void StatusPage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();

    uint8_t i, h;
    u8g_uint_t w, d;

    // °C
    u8g.setFont(u8g_font_profont12); 
    u8g.setPrintPos(51,21);
    u8g.write(0xB0);    u8g.print("C");
    u8g.drawStr(51, 31, "rpm"); 

    // btn
    drawButton(100,28,0, heaterStatus, u8g_font_5x8r);
    drawButton(100,40,1, motorStatus, u8g_font_trixel_square);
    drawButton(90,60,2, "->MENU", u8g_font_5x8r);
    if(ptTemperatureManager->THERMAL_RUNAWAY_FLAG)
        drawButton(85,61,3, "TRP", u8g_font_5x8r);
    if(ptTemperatureManager->COLD_EXTRUSION_FLAG)
        drawButton(55,61,2, "PCE", u8g_font_5x8r);
    //drawButton(85,61,3, "TRP", u8g_font_5x8r);
}


void SavePage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();

    uint8_t i, h;
    u8g_uint_t w, d;

    // btn
    drawButton(50,60,0, "save", u8g_font_5x8r);
    drawButton(90,60,1, "back", u8g_font_5x8r);
    //drawButton(55,61,2, "PCE", u8g_font_5x8r);
    //drawButton(85,61,3, "TRP", u8g_font_5x8r);
}

void ResetPage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();

    uint8_t i, h;
    u8g_uint_t w, d;

    // btn
    drawButton(50,60,0, "reset", u8g_font_5x8r);
    drawButton(90,60,1, "back", u8g_font_5x8r);
    //drawButton(55,61,2, "PCE", u8g_font_5x8r);
    //drawButton(85,61,3, "TRP", u8g_font_5x8r);
}