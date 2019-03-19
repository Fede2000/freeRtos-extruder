
#ifndef	EEPROMHELPER_h  //EEPROM : name conflict
#define EEPROMHELPER_h

#include <Arduino.h>
#include <EEPROM.h>
#include "configuration.h"

// EEPROM
void write16b( int value, int addr = 0);
int read16b(int addr);

void writeEprom(int temp, int speed);
void readEprom(double &temp, int &speed);

#endif