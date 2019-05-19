
#include <U8g2lib.h>
#include "Menu.h"
#include "Arduino.h"

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 23, /* data=*/ 17, /* CS=*/ 16, /* reset=*/ 8);


void Menu::drawMenu() 
{ 

    u8g2_uint_t i, h;
    u8g2_uint_t w, d;

    u8g2.setFont(menuFont);     //u8g_font_6x10
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop();
    h = u8g2.getAscent()-u8g2.getDescent();
    w = u8g2.getDisplayWidth();

    for( i = 0; i < itemIdx; i++ ) {
        d = (w-u8g2.getStrWidth(menu_strings[i]))/2;
        u8g2.setDrawColor(1);
        
        
        if ( i == currentMenu && isSelectable) {

            /*  DRAW box  centered */
            /*
            u8g2.drawBox(d-2, topSpacing + i*h, w-2*d+2, h);
            u8g2.setDefaultBackgroundColor();
            */
            if (isSelected){
                u8g2.setFont(u8g_font_profont15);  //u8g_font_6x12_75r
                u8g2.setFontRefHeightText();
                u8g2.setFontPosTop();
                u8g2.drawStr(0, topSpacing+i*h, "<");
                //u8g2.setPrintPos(2,topSpacing+i*h);
                //u8g2.print(0x20);    // https://github.com/olikraus/u8glib/wiki/fontgroupx11
                u8g2.setFont(menuFont);
                u8g2.setFontRefHeightText();
                u8g2.setFontPosTop();
            }
            else
                u8g2.drawStr(2, topSpacing+i*h, ">");
        }

        //u8g2.drawStr(d, topSpacing+i*h, menu_strings[i]); // centered
        u8g2.drawStr(10, topSpacing+i*h, menu_strings[i]);
        
        if(menu_values_float[i] != NULL || menu_values_double[i]  != NULL ){
            u8g2.setDrawColor(1);
            //u8g2.setPrintPos(d -d*2 + w + 3, topSpacing + i*h+1); centered
            u8g2.setCursor(10 -d*2 + w + 3, topSpacing + i*h+1);
            menu_values_float[i] != NULL ? u8g2.print(*menu_values_float[i], 0) : u8g2.print(*menu_values_double[i], 0);
        }

        //drawExtra();   
    } 
} 

void Page::drawTitle(){
    u8g2_uint_t h;
    u8g2_uint_t w, d;
    u8g2.setFontRefHeightText();
    u8g2.setFontPosBaseline();
    u8g2.setFont(u8g2_font_7x13B_mf);

    h = u8g2.getAscent()-u8g2.getDescent();
    w = u8g2.getDisplayWidth();    
    d = (w - u8g2.getStrWidth(title) )/2;
    u8g2.drawStr(d, h + topSpacing, title);  //centered
    u8g2.drawRFrame(0,0,w, h + topSpacing*2, 6);
    topSpacing = h + topSpacing*3;
}

void Page::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();  
}

void Page::drawButton(int x, int y, int id, char * name = NULL , const uint8_t *font = u8g2_font_7x13_mf){
    int h,d;
    u8g2.setFont(font);
    u8g2.setFontRefHeightText();
    u8g2.setFontPosBaseline();
    h = u8g2.getAscent()-u8g2.getDescent();
    d = u8g2.getStrWidth(name);
    if(id==currentMenu){  
        u8g2.drawRBox(x -3, y -h -3, d+2*3, h+3*2, 3 );
        u8g2.setDrawColor(0);
        u8g2.drawStr(x, y, name);
        u8g2.setDrawColor(1);
    }
    else{
        u8g2.drawStr(x, y, name);  
        //u8g2.drawFrame(x -2, y -h -2, d+2*2, h+2*2 );
    }
    
}


void StatusPage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    u8g2_uint_t i, h;
    u8g2_uint_t w, d;    

    u8g2.setFont(u8g2_font_6x12_tf);     //u8g_font_6x10
    u8g2.setFontRefHeightText();
    u8g2.setFontPosTop();
    h = u8g2.getAscent()-u8g2.getDescent();
    w = u8g2.getDisplayWidth();
    ///////////////
    i=0;
    char t_setpoint_ch[10];  //  Hold The Convert Data
    char t_ch[10];         //  Hold The Convert Data
    dtostrf(int(ptTemperatureManager->temperature),1,0,t_ch);
    dtostrf(int(ptTemperatureManager->tempSetpoint),1,0,t_setpoint_ch);
    char combined[32] = {0};
    //strcat(combined,"T:");
    strcat(combined, t_ch);
    strcat(combined, "/");
    strcat(combined, t_setpoint_ch);
    u8g2.setFont(u8g2_font_siji_t_6x10); 
    u8g2.drawGlyph(0, topSpacing+i*h, 57372);
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawStr(15, topSpacing+i*h, combined);
    u8g2.setCursor(15 + u8g2.getStrWidth(combined),topSpacing+i*h);
    u8g2.write(0xB0);    u8g2.print("C");

    i=1;
    char s_ch[10];         //  Hold The Convert Data
    dtostrf(int(ptExtruderManager->speed_rpm),1,0,s_ch);
    char combined2[32] = {0};
    //strcat(combined2,"S:");
    strcat(combined2, s_ch);
    strcat(combined2, " rpm");

    u8g2.setFont(u8g2_font_siji_t_6x10); 
    u8g2.drawGlyph(0, topSpacing+i*h, 57408);
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawStr(15, topSpacing+i*h, combined2);
    

    // btn
    int id=0;
    drawButton(100,28,id++, heaterStatus, u8g_font_5x8r);
    drawButton(100,40,id++, motorStatus, u8g_font_trixel_square);
    drawButton(90,60,id++, (char*)"->MENU", u8g_font_5x8r);
    if(ptTemperatureManager->THERMAL_RUNAWAY_FLAG && ptTemperatureManager->PREVENT_THERMAL_RUNAWAY_IS_ACTIVE ){
        u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); 
        u8g2.drawGlyph(10, 61, 71);
        u8g2.setFont(u8g2_font_6x12_tf);

        drawButton(20,61,id++,(char*)"PTR", u8g_font_5x8r);
    }
    if(ptTemperatureManager->COLD_EXTRUSION_FLAG && ptTemperatureManager->PREVENT_COLD_EXTRUSION_IS_ACTIVE ){ // && ptExtruderManager->is_step)
        u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); 
        u8g2.drawGlyph(45, 61, 71);
        u8g2.setFont(u8g2_font_6x12_tf);

        drawButton(55,61,id++,(char*)"PCE", u8g_font_5x8r);
    }
}

void SettingPage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();

    // btn
    #ifdef PREVENT_COLD_EXTRUSION
    drawButton(50,52,3, PCE, u8g_font_5x8r);
    #endif
    #ifdef PREVENT_THERMAL_RUNAWAY
    drawButton(10,52,2, PTR, u8g_font_5x8r);
    #endif
    drawButton(90,60,4, (char*)"<-BACK", u8g_font_5x8r);  //TODO: se non definite id non deve essere 4
}


void SavePage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();

    // btn
    drawButton(50,60,0, (char*)"save", u8g_font_5x8r);
    drawButton(90,60,1, (char*)"back", u8g_font_5x8r);
}

void ResetPage::drawPage(){
    topSpacing = 3;
    if(title !=NULL)
        drawTitle();

    if(has_menu)
        drawMenu();
    // btn
    drawButton(50,60,0, (char*)"reset", u8g_font_5x8r);
    drawButton(90,60,1, (char*)"back", u8g_font_5x8r);
}