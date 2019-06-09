#include "EepromHelper.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "configuration.h"


// EEPROM
void write16b( int value, int addr = 0){
  Serial.println(value);
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

void writeEprom(int temp, int speed, int steps, bool retractionIsEnsabled){
  write16b(temp,4);
  write16b(speed,8);
  write16b(steps, 12);
  write16b(retractionIsEnsabled,16);
  EEPROM.write(ADDRESS_CK, EEPROM_CK_VALUE);
}
void readEprom(double &temp, float &speed, int &steps, bool &retractionIsEnsabled){
  temp = read16b(4);
  speed = read16b(8);
  steps = read16b(12);
  retractionIsEnsabled = read16b(16);

}
