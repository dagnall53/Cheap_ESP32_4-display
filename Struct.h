#ifndef _STRUCT_H_
#define _STRUCT_H_
#include <Arduino_GFX_Library.h>

struct MySettings {  //key,default page,ssid,PW,Displaypage, UDP,Mode,serial,Espnow
  int EpromKEY;      // Key is changed to allow check for clean EEprom and no data stored change in the default will result in eeprom being reset
  int DisplayPage;   // start page after defaults (currently -100 to show fonts data)
  char ssid[16];
  char password[16];
  char UDP_PORT[5]; // hold udp port as char to make using keyboard easier for now. use atoi when needed!!
  bool UDP_ON;
  bool Serial_on;
  bool ESP_NOW_ON;
};
struct MyColors{
  uint16_t TextColor;
  uint16_t BackColor;
  uint16_t BorderColor;

};


// helpers for sine cos?
int getSine(int angle);
int getCosine(int angle);

#endif