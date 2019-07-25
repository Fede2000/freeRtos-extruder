
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
void Page::drawIconButton(int x, int y, int id, uint16_t code = NULL , const uint8_t *font = u8g2_font_7x13_mf){
    int h,d;
    u8g2.setFont(font);
    h = u8g2.getAscent()-u8g2.getDescent();
    d = u8g2.getMaxCharWidth();
    if(id==currentMenu){ 
        if(isSelected){
            u8g2.drawRFrame(x -3, y -h -3, d+2*3, h+3*2, 5 );
            u8g2.setDrawColor(1);
            u8g2.drawGlyph(x, y, code);
        } 
        else{
            u8g2.drawRBox(x -3, y -h -3, d+2*3, h+3*2, 5 );
            u8g2.setDrawColor(0);
            u8g2.drawGlyph(x, y, code);
            u8g2.setDrawColor(1);
        }
        
    }
    else{
        u8g2.drawGlyph(x, y, code); 
        //u8g2.drawFrame(x -2, y -h -2, d+2*2, h+2*2 );
    }
    
}


void StatusPage::drawPage(){
    topSpacing = 3;
    int id=0;   // ->itemId

    if(title !=NULL)
        drawTitle();

    u8g2_uint_t i, h;
    u8g2_uint_t w, d;    

    u8g2.setFont(u8g2_font_6x12_tf);     //u8g_font_6x10
    u8g2.setFontRefHeightText();
    //u8g2.setFontPosTop();
    h = u8g2.getAscent()-u8g2.getDescent();
    w = u8g2.getDisplayWidth();
    
    /////////////// first row
    int x = 18;
    int y = 3 + topSpacing+h;
    char t_setpoint_ch[10];  //  Hold The Convert Data
    char t_ch[10];         //  Hold The Convert Data
    dtostrf(int(ptTemperatureManager->temperature),1,0,t_ch);
    dtostrf(int(ptTemperatureManager->tempSetpoint),1,0,t_setpoint_ch);
    char combined[32] = {0};
    //strcat(combined,"T:");
    strcat(combined, t_ch);
    strcat(combined, "/");
    //strcat(combined, t_setpoint_ch);
    //u8g2.setFont(u8g2_font_siji_t_6x10); 
    //u8g2.drawGlyph(0, topSpacing+i*h, 57372);
     if(ptTemperatureManager->HEATER_ENABLED)
        drawIconButton(0, y ,id++, 57372, u8g2_font_siji_t_6x10);   //is heating
    else
        drawIconButton(0, y ,id++, 57357, u8g2_font_siji_t_6x10);   //no warming down
    
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawStr(x, y, combined);
    x +=u8g2.getStrWidth(combined);
    drawButton(x, y, id++, t_setpoint_ch, u8g2_font_6x12_tf);
    x += u8g2.getStrWidth(t_setpoint_ch) + 8;
    u8g2.setCursor(x,y);
    u8g2.write(0xB0);   u8g2.print("C");
    u8g2.setCursor(105,y);
    #ifdef DEBUG
        //u8g2.print(int(ptTemperatureManager->output));
        u8g2.print(int(ptExtruderManager->timer));
    #endif
    //drawButton(110, y, 1236, heaterStatus, u8g_font_trixel_square);

    /////////////// second row
    y = y + h + 4;
    char s_ch[10];         //  Hold The Convert Data
    dtostrf(int(ptExtruderManager->speed_rpm),1,0,s_ch);
   
    drawIconButton(0, y,id++, 57408, u8g2_font_siji_t_6x10);
    //u8g2.setFont(u8g2_font_6x12_tf);
    drawButton(18, y, id++, s_ch, u8g2_font_6x12_tf);
    u8g2.drawStr(18 + u8g2.getStrWidth(s_ch) + 8, y, "rpm");
    
    
    
    /////////////// third row
    y = 6 + y + h;
    if(ptExtruderManager->is_retraction_enabled)
        drawIconButton(2, y, id++,79, u8g2_font_open_iconic_embedded_1x_t);   //retraction button
    else
        drawIconButton(2, y, id++,78, u8g2_font_open_iconic_arrow_1x_t);   //no retraction button

    //drawButton(100,40,id++, motorStatus, u8g_font_trixel_square);
    
    char st_ch[10];         //  Hold The Convert Data
    dtostrf(ptExtruderManager->steps_to_retract,1,0,st_ch);
    u8g2.setFont(u8g2_font_6x12_tf);
    drawButton(18, y, id++, st_ch, u8g2_font_6x12_tf);
    ///////////////
    //drawIconButton(117,61,id++,84, u8g2_font_open_iconic_arrow_1x_t);   //save button
    

    if(ptTemperatureManager->THERMAL_RUNAWAY_FLAG && ptTemperatureManager->PREVENT_THERMAL_RUNAWAY_IS_ACTIVE ){
        u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); 
        u8g2.drawGlyph(75, 61, 71); //allert icon
        u8g2.setFont(u8g2_font_6x12_tf);

        drawButton(85,61,id++,(char*)"PTR", u8g_font_5x8r);
    }
    if(ptTemperatureManager->COLD_EXTRUSION_FLAG && ptTemperatureManager->PREVENT_COLD_EXTRUSION_IS_ACTIVE ){ // && ptExtruderManager->is_input_step)
        u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t); 
        u8g2.drawGlyph(45, 61, 71); //allert icon
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