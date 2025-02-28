#ifndef _STRUCT_H_
#define _STRUCT_H_

struct MySettings {  //key,default page,ssid,PW,Displaypage, UDP,Mode,serial,Espnow,Listtext size
  int EpromKEY;      // Key is changed to allow check for clean EEprom and no data stored change in the default will result in eeprom being reset
  int DisplayPage;   // start page after defaults (currently -100 to show fonts data)
  char ssid[16];
  char password[16];
  char UDP_PORT[5]; // hold udp port as char to make using keyboard easier for now
  bool UDP_ON;
  bool Serial_on;
  bool ESP_NOW_ON;
};


// helpers for sine cos?
int getSine(int angle);
int getCosine(int angle);

#endif