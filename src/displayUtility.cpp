#include "U8glib.h"
#include "displayUtility.h"


  
void Menu::drawMenu() 
{ 
    uint8_t i, h;
    u8g_uint_t w, d;
    int spacingTop = 3;
    h = ptU8g->getFontAscent()-ptU8g->getFontDescent();
    w = ptU8g->getWidth();
    //draw title
    if(title !=NULL){
        ptU8g->setFont(u8g_font_7x13);
        d = (w-ptU8g->getStrWidth(title))/2;
        ptU8g->drawStr(d, h + spacingTop, title);
        ptU8g->drawFrame(0,0,w, h + spacingTop*2);
        spacingTop = h + spacingTop*3;
    }

    ptU8g->setFont(u8g_font_6x10);
    ptU8g->setFontRefHeightText();
    ptU8g->setFontPosTop();
    h = ptU8g->getFontAscent()-ptU8g->getFontDescent();
    w = ptU8g->getWidth();

    for( i = 0; i < itemIdx; i++ ) {
        d = (w-ptU8g->getStrWidth(menu_strings[i]))/2;
        ptU8g->setDefaultForegroundColor();
        if ( i == curruntMenu && isSelectable) {
            ptU8g->drawBox(d-2, spacingTop + i*h, w-2*d+2, h);
            ptU8g->setDefaultBackgroundColor();
        }
        ptU8g->drawStr(d, spacingTop+i*h, menu_strings[i]);
        
        if(menu_values_int[i] != NULL || menu_values_double[i]  != NULL ){
            ptU8g->setDefaultForegroundColor();
            ptU8g->setPrintPos(d -d*2 + w + 3, spacingTop + i*h+1);
            menu_values_int[i] != NULL ? ptU8g->print(*menu_values_int[i]) : ptU8g->print(*menu_values_double[i]);
        }   
    } 
} 


 void Menu::updateMenu(int aUiKeyCode) {
 
    switch ( aUiKeyCode ) {
        case 1:
            //aUiKeyCode = KEY_NONE;
            curruntMenu++;
            if ( curruntMenu >= itemIdx )
                
                curruntMenu = 0;
            //menu_redraw_required = 1;
        break;

        case -1:
            //aUiKeyCode = KEY_NONE;
            if ( curruntMenu == 0 )
                curruntMenu = itemIdx;
            curruntMenu--;
            //menu_redraw_required = 1;
        break;
    }
}