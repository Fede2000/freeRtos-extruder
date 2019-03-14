#include "U8glib.h"
#include "displayUtility.h"


  
void Menu::drawMenu() 
{ 
  uint8_t i, h;
  u8g_uint_t w, d;

  ptU8g->setFont(u8g_font_6x13);
  ptU8g->setFontRefHeightText();
  ptU8g->setFontPosTop();
  
  h = ptU8g->getFontAscent()-ptU8g->getFontDescent();
  w = ptU8g->getWidth();
  for( i = 0; i < itemIdx; i++ ) {
    d = (w-ptU8g->getStrWidth(menu_strings[i]))/2;
    ptU8g->setDefaultForegroundColor();
    if ( i == curruntMenu && isSelectable) {
      ptU8g->drawBox(d-2, i*h+1, -d*2 + w, h);
      ptU8g->setDefaultBackgroundColor();
    }
    ptU8g->drawStr(d, i*h, menu_strings[i]);
    
    if(menu_values_int[i] != NULL || menu_values_double[i]  != NULL ){
        ptU8g->setDefaultForegroundColor();
        ptU8g->setPrintPos(d -d*2 + w+5, i*h+1);
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