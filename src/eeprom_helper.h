
#include <Arduino.h>
#include <EEPROM.h>

#ifndef	EEPROM_HELPER_h  //EEPROM : name conflict
#define EEPROM_HELPER_h


// EEPROM
write16b( int value, int addr = 0){
  char * pt = (char *) &value;
  EEPROM.write(addr++, *pt++);
  EEPROM.write(addr , *pt );
}
int read16b(int addr){
  int dato = 0;
  char * pt = (char *) &dato;
  *pt++ = EEPROM.read(addr++);
  *pt++ = EEPROM.read(addr);
   return dato;
}

void writeEprom(int temp, int speed){
  write16b(temp,4);
  write16b(speed,8);
  EEPROM.write(address_ck, eeprom_ck_value);
}
void readEprom(double &temp, int &speed){
  temp = read16b(4);
  speed = read16b(8);
}

#endif