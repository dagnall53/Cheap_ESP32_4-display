/*******************************************************************************

From scratch build! 
 for keyboard  trying to use elements of https://github.com/fbiego/esp32-touch-keyboard/tree/main
*/
// * Start of Arduino_GFX setting


//******  Wifi stuff 
#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP Udp;
#define BufferLength 500
bool line_1;
char nmea_1[500];

bool line_U = false;
char nmea_U[BufferLength];  // NMEA buffer for UDP input port
// *************  ESP-NOW variables and functions in ESP_NOW_files************
bool line_EXT;
char nmea_EXT[500];
bool EspNowIsRunning = false;




#include <Arduino_GFX_Library.h>
//select pin definitions for installed GFX version (test with 1.3.8. and 1.3.1)
// Original version for GFX 1.3.1 only. #include "GUITIONESP32-S3-4848S040_GFX_133.h"
#include "Esp32_4inch.h"

//*********** for keyboard*************
#include "Keyboard.h"


#include "Touch.h"
TAMC_GT911 ts = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
/*******************************************************************************
 * Touch settings- 
 ******************************************************************************/
// #include <TAMC_GT911.h>
// #define TOUCH_ROTATION ROTATION_NORMAL
// #define TOUCH_MAP_X1 480
// #define TOUCH_MAP_X2 0
// #define TOUCH_MAP_Y1 480
// #define TOUCH_MAP_Y2 0

// TAMC_GT911 ts = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);



#include <EEPROM.h>
#include "fonts.h"
#include "FreeMonoBold8pt7b.h"
#include "FreeMonoBold27pt7b.h"
#include "FreeSansBold30pt7b.h"
#include "FreeSansBold50pt7b.h"
//For SD card (see display page -98 for test)

#include <SD.h> // pins set in GFX .h 
#include "SPI.h"
#include "FS.h"

//jpeg
#include "JpegFunc.h"
#define JPEG_FILENAME_LOGO "/logo.jpg" // logo in jpg on sd card
//audio 
#include "Audio.h"
#define AUDIO_FILENAME_01 "/ChildhoodMemory.mp3"
#define AUDIO_FILENAME_02 "/SoundofSilence.mp3"
#define AUDIO_FILENAME_03 "/MoonlightBay.mp3"
#define AUDIO_FILENAME_START "/ship-bell.mp3"
//set up Audio
Audio audio;


//**   structures and defauts for my variables 
#include "Struct.h"
// struct MySettings {  //key,default page,ssid,PW,Displaypage, UDP,Mode,serial,Espnow,Listtext size
//   int EpromKEY;      // Key is changed to allow check for clean EEprom and no data stored change in the default will result in eeprom being reset
//   int DisplayPage;   // start page after defaults (currently -100 to show fonts data)
//   char ssid[16];
//   char password[16];
//   char UDP_PORT[5]; // hold udp port as char to make using keyboard easier for now
//   bool UDP_ON;
//   bool Serial_on;
//   bool ESP_NOW_ON;
// };
int UDP_PORT_NUMBER;
// use page -100 for any swipe testing 
// change key (first parameter) to set defaults
MySettings Default_Settings = {5, 0, "GUESTBOAT", "12345678", "2002", false, true, true };
MySettings Saved_Settings;
MySettings Current_Settings;
struct Displaysettings {
  bool keyboard, CurrentSettings;
  bool NmeaDepth, nmeawind, nmeaspeed, GPS;
};
Displaysettings Display_default = { false, true, false, false, false, false };
Displaysettings Display_Setting;

struct BarChart {
  int topleftx, toplefty, width, height, bordersize, value, rangemin, rangemax, visible; 
  uint16_t barcolour;
};

struct Button { int h, v, width, height, bordersize;
uint16_t backcol,textcol,bordercol;
bool Keypressed,KeySent;
unsigned long LastDetect; 
};
Button defaultbutton = {20,20,75,75,2,BLACK,WHITE,BLUE,false,0};

struct TouchMemory {
  int consecutive;
  unsigned long sampletime;
  uint8_t x;
  uint8_t y;
  uint8_t size;
  int X3Swipe;
  int Y3Swipe;
};
TouchMemory TouchData;

int _null, _temp;  //null pointers
struct NMEA_DATA{
int depth,sog,stw,windspeed,winddir;};




String Fontname;
int text_height = 12;      //so we can get them if we change heights etc inside functions
int text_offset = 12;      //offset is not equal to height, as subscripts print lower than 'height'
int text_char_width = 12;  // useful for monotype? only NOT USED YET! Try gfx->getTextBounds(string, x, y, &x1, &y1, &w, &h);


//colour order  background, text, border 
Button TopLeftbutton = {0,0,75,75,5,BLUE,WHITE,BLACK,false,0};
Button TopRightbutton = {405,0,75,75,5,BLUE,WHITE,BLACK,false,0};
Button BottomRightbutton = {405,405,75,75,5,BLUE,WHITE,BLACK,false,0};
Button BottomLeftbutton = {0,405,75,75,5,BLUE,WHITE,BLACK,false,0};

Button SSID ={30,100,420,35,5,WHITE,BLACK,BLUE,false};
Button PASSWORD={30,140,420,35,5,WHITE,BLACK,BLUE,false};
Button UDPPORT={30,180,420,35,5,WHITE,BLACK,BLUE,false}; 
//for selections
Button Full0Center ={80,0,320,75,5,BLUE,WHITE,BLACK,false,0};
Button Full1Center ={80,80,320,75,5,BLUE,WHITE,BLACK,false,0};
Button Full2Center ={80,160,320,75,5,BLUE,WHITE,BLACK,false,0};
Button Full3Center ={80,240,320,75,5,BLUE,WHITE,BLACK,false,0};
Button Full4Center ={80,320,320,75,5,BLUE,WHITE,BLACK,false,0};
Button Full5Center ={80,400,320,75,5,BLUE,WHITE,BLACK,false,0};

#define  On_Off   ? "OFF":"ON"



// Draw the compass pointer at an angle in degrees
void DrawCompass(int x,int y, int rad ){
  //Work in progress!
  int Start, dot, colorbar, bigtick;
  Start=rad*0.83; //200
  dot= rad*0.86;  //208
  colorbar=rad*0.91; //220
  bigtick =rad-1;  //239
  //rad =240 example dot is 200 to 208   bar is 200 to 239 wind colours 200 to 220
  // colour segments
        gfx->fillArc(x,y,colorbar,Start,270-45,270,RED);
        gfx->fillArc(x,y,colorbar,Start,270,270+45,GREEN);
//Mark 12 linesarks at 30 degrees
        for (int i=0;i<(360/30);i++){gfx->fillArc(x,y,bigtick,Start,i*30,(i*30)+1,BLACK); } //239 to 200
        for (int i=0;i<(360/10);i++){gfx->fillArc(x,y,dot,Start,i*10,(i*10)+1,BLACK); } // dots at 10 degrees
}

bool WindpointToBoat=false;
void drawCompassPointer(int x,int y, int rad, int angle, uint16_t COLOUR,bool to) {
  int x_offset, y_offset,tail1x,tail1y,tail2x,tail2y;  // The resulting offsets from the center point
 // Normalize the angle to the range 0 to 359
  float inner,outer;


  while (angle < 0) angle += 360;
  while (angle > 359) angle -= 360;
     // rose radius is default at 100 and returns 100*sine.. so may need to make offsets 2x bigger for 200 size triangel ?
  //if (rad=240){inner=0.6;outer=2;}else{inner=0.3;outer=1;}
  inner=(rad*98)/400; // just a tiny bit smaller!
  outer=(rad*98)/120;
     if (to){ //Point to boat from wind
  x_offset =x+ ((outer*getSine(angle))/100);
  y_offset =y+ ((outer*getCosine(angle))/100);
  tail1x   =x+((inner*getSine(angle+20))/100);
  tail1y   =y+((inner*getCosine(angle+20))/100);
  tail2x   =x+((inner*getSine(angle-20))/100);
  tail2y   =y+ ((inner*getCosine(angle-20))/100);}
  else{
  x_offset =x+((inner*getSine(angle))/100);
  y_offset =y+((inner*getCosine(angle))/100);
  tail1x   =x+((outer*getSine(angle+7))/100);
  tail1y   =y+((outer*getCosine(angle+7))/100);
  tail2x   =x+((outer*getSine(angle-7))/100);
  tail2y   =y+ ((outer*getCosine(angle-7))/100);

  }
  // The actual drawing command will depend on your display library
  /*fillTriangle(int16_t x0, int16_t y0,
                               int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
  */
  gfx->fillTriangle(x_offset,y_offset,tail1x,tail1y,tail2x,tail2y, COLOUR);
  
}

void WindArrow(int x,int y, int rad ,int wind ,bool to){
  static int lastwind;
      drawCompassPointer(x,y, rad, lastwind, BLUE,to);
      drawCompassPointer(x,y,rad, wind, RED,to);  
      lastwind=wind;

}

void Display(int page) { // setups for alternate pages to be selected by page.
  static int LastPageselected;
  // some local variables for tests;
  static int font;
  static int SwipeTestLR, SwipeTestUD,volume;
  static bool RunSetup;
  static unsigned int slowdown,timer2;
  static float wind;
  float temp,oldtemp;
  char Tempchar[30];
  String tempstring; 
  int FS =1; // for font size test

  if (page != LastPageselected) { RunSetup = true; }
  //generic stuff  for ALL pages
  if (RunSetup){      
        gfx->fillScreen(BLUE);
        gfx->setTextColor(WHITE);
        setFont(0);
        }
  // add any generic stuff here like check for left right? 

  // Now specific stuff for each page

  switch (page) {
    case -101:  //a test for Swipes 
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE);
        font = 0;
        SwipeTestLR = 0;
        SwipeTestUD = 0;
        setFont(font);
        GFXBoxPrintf(0, 250, 3, "-swipe test- ");
      }

      if (millis() >= slowdown + 10000) {
        slowdown = millis();
        gfx->fillScreen(BLACK);
        GFXBoxPrintf(0, 50, 2, "Periodic blank ");
      }

      // if (Swipe2(SwipeTestLR, 1, 10, true, SwipeTestUD, 0, 10, false, false)) {
      //   Serial.printf(" LR updated %i UD updated %i \n", SwipeTestLR, SwipeTestUD);  // swipe page using (current) TouchData information with LR (true) swipe
      //   GFXBoxPrintf(0, 0, 1, "SwipeTestLR (%i)", SwipeTestLR);
      //   GFXBoxPrintf(0, 480 - text_height, 1, "SwipeTestUD (%i)", SwipeTestUD);
      // };

      break;
          case -99:  //a test for Screen Colours / fonts 
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE);
        font = 0;
        SwipeTestLR = 0;
        SwipeTestUD = 0;
        setFont(font);
        GFXBoxPrintf(0, 250, 3, "-TEST Colours- ");
      }

      if (millis() >= slowdown + 10000) {
        slowdown = millis();
        gfx->fillScreen(BLACK);
        font=font+1; if (font >10) {font=0;}
        GFXBoxPrintf(0, 50, 2, "Colours check");
              GFXBoxPrintf(0, 100, FS, "Font size %i",FS);
      DisplayCheck(true);
      }
      


      break;

    case -102:
     if (RunSetup) {
        GFXBoxPrintf(0, 140, 2,WHITE,BLUE, " Testing new buttons\n");
        //gfx->println(" use three finger touch to renew dir listing");

      }
      if (millis() >= slowdown + 10000) {
        slowdown = millis();
        gfx->fillScreen(BLUE); // clean up redraw buttons
     // GFXBorderBoxPrintf(TopLeftbutton,1,"LEFT");
     // GFXBorderBoxPrintf(TopRightbutton,1,"RIGHT");      
      }
      if (ts.isTouched) {
          TouchCrosshair(20); }
        if (CheckButton(TopLeftbutton)){volume=volume+1; if(volume>21){volume=0;};
          Serial.println("LEFT");GFXBoxPrintf(80, 0, 2, WHITE,BLUE, " LEFT   %i\n",volume);audio.setVolume(volume);delay(300);}
        if (CheckButton(TopRightbutton)){volume=volume-1;if(volume<1){volume=21;};
          Serial.println("RIGHT");GFXBoxPrintf(80, 0,2, WHITE,BLUE, " RIGHT  %i\n",volume);audio.setVolume(volume);delay(300);}
      if  (!audio.isRunning()){ audio.connecttoFS(SD, AUDIO_FILENAME_02);}
      if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
               
     break;
     case -10:  // a test page for fonts
      if (RunSetup) {
         GFXBorderBoxPrintf(Full0Center, "-Font test -");

      }
      if (millis() > slowdown + 3500) {
        slowdown = millis();
        gfx->fillScreen(BLUE);
        //wind=wind+9; if (wind>360) {wind=0;gfx->fillScreen(BLUE);} 
        font=font+1;if (font>15) {font=0;} 
        //setFont(0);
        // dataline(1, Current_Settings, "Current"); 
        temp=12.3;
        Serial.print("about to set");Serial.print(font); 
        setFont(font);
        Serial.print(" setting fontname");Serial.println(Fontname);
      }
        if (millis() > timer2 + 500) {
        timer2 = millis();
       temp= random(-9000, 9000);
       temp=temp/1000;
       setFont(font); 
       Fontname.toCharArray(Tempchar,30,0);
       setFont(3);
       GFXBorderBoxPrintf(0,0,480,75,1,5,BLUE,WHITE,BLACK, "FONT:%i name%s",font,Tempchar); 
       setFont(font); 
       GFXBorderBoxPrintf(0,150,480,330,1,5,BLUE,WHITE,BLUE, "Test %4.2f height<%i>",temp,text_height);  
      // if((2*text_height)<180){GFXBorderBoxPrintf(0,160,480,(2*text_height)+10,2,5,BLUE,WHITE,BLACK, "two X %4.2f",temp); } 
      //  if((3*text_height)<180){GFXBorderBoxPrintf(0,265,480,(3*text_height)+10,3,5,BLUE,WHITE,BLACK, "%4.2f",temp);  }
        }
        if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=0;delay(100);}
      break;

    

    case -4:
      if (RunSetup) {
         GFXBorderBoxPrintf(Full0Center, "Set UDP PORT");
        keyboard(-1);  //reset
        keyboard(0);
      }
      
      Use_Keyboard(Current_Settings.UDP_PORT, sizeof(Current_Settings.UDP_PORT));
      if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=-1;delay(300);}
      if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
      break;


    case -3:
      if (RunSetup) {
         GFXBorderBoxPrintf(Full0Center, "Set Password");
        keyboard(-1);  //reset
       
        keyboard(0);
      }
      
      Use_Keyboard(Current_Settings.password, sizeof(Current_Settings.password));
      if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=-1;delay(300);}
      if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
      break;


    case -2:
      if (RunSetup) {
         GFXBorderBoxPrintf(Full0Center, "Set SSID");
        keyboard(-1);  //reset    
        keyboard(0);
      }
      
      Use_Keyboard(Current_Settings.ssid, sizeof(Current_Settings.ssid));
      if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
      if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=-1;delay(300);}
      break;
    case -1:  // this is the main WIFI settings page 
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(1);
        setFont(4);
        gfx->setCursor(180, 180);      
        GFXBorderBoxPrintf(SSID,1,Current_Settings.ssid);
        GFXBorderBoxPrintf(PASSWORD,1,Current_Settings.password);
        GFXBorderBoxPrintf(UDPPORT,1,Current_Settings.UDP_PORT);
         //GFXBorderBoxPrintf(Full4Center, "Mac: Fonts");
        setFont(0);
      dataline(1, Current_Settings, "Current");
      setFont(1);
      //while (ts.sTouched{yield(); Serial.println("yeilding -1");}
      }
         if (millis() > slowdown + 1000) {
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
        long rssiValue = WiFi.RSSI();
        GFXBorderBoxPrintf(Full4Center, " WIfi %i",rssiValue);
        gfx->setCursor(80,380);
        gfx->print(" IP:");gfx->println(WiFi.localIP());
       }
 

        if (CheckButton(SSID)){Current_Settings.DisplayPage=-2;};
        if (CheckButton(PASSWORD)){Current_Settings.DisplayPage=-3;};
        if (CheckButton(UDPPORT)){Current_Settings.DisplayPage=-4;};
        //if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=Current_Settings.DisplayPage+1;delay(300);}
        if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
      
      break;
    case 0:
      if (RunSetup) {
        setFont(3);//GFXBorderBoxPrintf(int h, int v, int width, int height, int textsize, int bordersize, 
        //uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
        
        GFXBorderBoxPrintf(TopLeftbutton,"Page-");
        GFXBorderBoxPrintf(TopRightbutton,"Page+");
        setFont(3);
        GFXBorderBoxPrintf(Full0Center, "-Top page-");
        GFXBorderBoxPrintf(Full1Center, "Check SD");
        GFXBorderBoxPrintf(Full2Center, "Set WIFI");
        GFXBorderBoxPrintf(Full3Center, "See NMEA");
        GFXBorderBoxPrintf(Full4Center, "Check Fonts");
        GFXBorderBoxPrintf(Full5Center, "Reset");
       
      }
      if (millis() > slowdown + 1000) {
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
       }
       if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
       if (CheckButton(Full1Center)){Current_Settings.DisplayPage=1;delay(100);}
       if (CheckButton(Full2Center)){Current_Settings.DisplayPage=-1;delay(100);}
       if (CheckButton(Full3Center)){Current_Settings.DisplayPage=4;delay(100);}
       if (CheckButton(Full4Center)){Current_Settings.DisplayPage=-10;delay(100);}
       if (CheckButton(Full5Center)){Current_Settings.DisplayPage=-1;delay(100);}

      break;
    case 1:
      if (RunSetup) {
        setFont(3);
        GFXBorderBoxPrintf(Full0Center,"SD Contents Run audio");
        gfx->setTextColor(WHITE, BLUE);
         
         GFXBorderBoxPrintf(BottomLeftbutton,"vol-");
         GFXBorderBoxPrintf(BottomRightbutton,"vol+");
         setFont(1);
        gfx->setCursor(0, 50);gfx->setTextSize(1);
        listDir(SD, "/", 0);
       }
     
        if  (!audio.isRunning()){ audio.connecttoFS(SD, AUDIO_FILENAME_02);}

        if (CheckButton(Full0Center)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=0;delay(100);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=0;delay(100);}
         if (CheckButton(BottomRightbutton)){volume=volume+1; if(volume>21){volume=21;};
            audio.setVolume(volume);}
        if (CheckButton(BottomLeftbutton)){volume=volume-1;if(volume<1){volume=0;};
            audio.setVolume(volume);}
 
      
      break;
    
    case 2:
    if (RunSetup) {
        GFXBoxPrintf(0,480-(text_height*2),  2, "PAGE 2 -SPEED");
      }
      if (millis() > slowdown + 1000) {
        //gfx->fillScreen(BLUE);
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
        wind=wind+1; if (wind>360) {wind=0;}
        temp=wind/10.0;
       // dataline(1, Current_Settings, "Current");
      
        setFont(11);
     // GFXBoxPrintf(0,150,  2, BLUE,BLUE, "%4.2f", oldtemp);  
      GFXBoxPrintf(0,150,  2, BLACK,BLUE, "%4.2f", temp);
      oldtemp=temp;  
      setFont(0);
      }
      

        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=Current_Settings.DisplayPage-1;delay(300);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=Current_Settings.DisplayPage+1;delay(300);}


      break;
case 3:
    if (RunSetup) {
        setFont(4);
        DrawCompass(240,240,240);
      
      }
      if (millis() > slowdown + 500) {
        slowdown = millis();
        WindArrow(240,240,240,wind,WindpointToBoat);
        wind=wind+13; if (wind>360) {wind=0; WindpointToBoat=!WindpointToBoat;}
        
        //Box in middle for wind dir / speed
        //int h, int v, int width, int height, int textsize, int bordersize, uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
        GFXBorderBoxPrintf(240-70,240-40, 140,80, 2,5,BLUE,BLACK,BLACK, "%3.0f",wind);
       // dataline(1, Current_Settings, "Current");
      }

        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=Current_Settings.DisplayPage-1;delay(300);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=4;delay(300);} //loop to page 1


      break;
      case 4:   // Quad display
    if (RunSetup) {
        setFont(5);
        //GFXBoxPrintf(0,480-(text_height*2),  1,BLACK,BLUE, "quad display"); 
        DrawCompass(360,120,120);    
        GFXBorderBoxPrintf(0,0, 235,235, 1,5,BLUE,BLACK,BLACK, "SPEED",wind/24);
        GFXBorderBoxPrintf(0,240, 235,235, 1,5,BLUE,BLACK,BLACK, "DEPTH",wind/3);
        //GFXBorderBoxPrintf(240,0, 235,235, 1,5,BLUE,BLACK,BLACK, "c%3.2F",wind*1.1);
        GFXBorderBoxPrintf(240,240, 235,235, 1,5,BLUE,BLACK,BLACK, "SOG",wind/11);
        delay(500);

      }
      if (millis() > slowdown + 300) {
        slowdown = millis();
        wind=wind+13; if (wind>360) {wind=0;WindpointToBoat=!WindpointToBoat;}  // sikmulate 4 boxes
        
        WindArrow(360,120,120,wind,true);
        // full writes including borders etc
        // GFXBorderBoxPrintf(0,0, 235,235, 1,5,BLUE,BLACK,BLACK, "%4.2fKts",wind/24);
        // GFXBorderBoxPrintf(0,240, 235,235, 1,5,BLUE,BLACK,BLACK, "%4.1fM",wind/3);
        // //GFXBorderBoxPrintf(240,0, 235,235, 1,5,BLUE,BLACK,BLACK, "c%3.2F",wind*1.1);
        // GFXBorderBoxPrintf(240,240, 235,235, 1,5,BLUE,BLACK,BLACK, "%3.1Fkts",wind/13);

        UpdateBB_DATA(0,0, 235,235, 1,5,BLUE,BLACK,BLACK, "%4.2fKts",wind/24);
        UpdateBB_DATA(0,240, 235,235, 1,5,BLUE,BLACK,BLACK, "%4.1fM",wind/3);
        //UpdateBB_DATA(240,0, 235,235, 1,5,BLUE,BLACK,BLACK, "c%3.2F",wind*1.1);
        UpdateBB_DATA(240,240, 235,235, 1,5,BLUE,BLACK,BLACK, "%3.1Fkts",wind/13);
       // dataline(1, Current_Settings, "Current");
      }

//        TouchCrosshair(20); 
        if (CheckButton(TopLeftbutton)){Current_Settings.DisplayPage=Current_Settings.DisplayPage-1;delay(300);}
        if (CheckButton(TopRightbutton)){Current_Settings.DisplayPage=1;delay(300);} //loop to page 1
 

      break;

default:
     if (RunSetup) {
        setFont(3);//GFXBorderBoxPrintf(int h, int v, int width, int height, int textsize, int bordersize, 
        //uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
        GFXBorderBoxPrintf(Full0Center, "-Top page-");
        GFXBorderBoxPrintf(TopLeftbutton,"Page-");
        GFXBorderBoxPrintf(TopRightbutton,"Page+");

        GFXBorderBoxPrintf(Full1Center, "Check SD");
        GFXBorderBoxPrintf(Full2Center, "Set WIFI");
        GFXBorderBoxPrintf(Full3Center, "See NMEA");
        GFXBorderBoxPrintf(Full4Center, "Check Fonts");
       
      }
      if (millis() > slowdown + 1000) {
        slowdown = millis();
       }
       if (CheckButton(Full1Center)){Current_Settings.DisplayPage=1;delay(100);}
       if (CheckButton(Full2Center)){Current_Settings.DisplayPage=-1;delay(100);}
       if (CheckButton(Full3Center)){Current_Settings.DisplayPage=4;delay(100);}
       if (CheckButton(Full4Center)){Current_Settings.DisplayPage=-10;delay(100);}

      break;
  }
  LastPageselected = page;
  RunSetup = false;
}


void setFont(int font) {

  switch (font) { //select font and automatically set height/offset based on character '['
    // set the heights and offset to print [ in boxes. Heights in pixels are NOT the point heights!
    case 0:  // SMALL 8pt
      Fontname = "FreeMono8pt7b";
      gfx->setFont(&FreeMono8pt7b);
      text_height = (FreeMono8pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono8pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;
      break;
    case 1:  // standard 12pt
    Fontname = "FreeMono12pt7b";
      gfx->setFont(&FreeMono12pt7b);
      text_height = (FreeMono12pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono12pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;

      break;
    case 2:  //standard 18pt
    Fontname = "FreeMono18pt7b";
      gfx->setFont(&FreeMono18pt7b);
      text_height = (FreeMono18pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono18pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;

      break;
      case 3:  //BOLD 8pt
    Fontname = "FreeMonoBOLD8pt7b";
      gfx->setFont(&FreeMonoBold8pt7b);
      text_height = (FreeMonoBold8pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold8pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;

      break;
    case 4:  //BOLD 12pt
    Fontname = "FreeMonoBOLD12pt7b";
      gfx->setFont(&FreeMonoBold12pt7b);
      text_height = (FreeMonoBold12pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold12pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;

      break;
    case 5:  //BOLD 18 pt
      Fontname = "FreeMonoBold18pt7b";
      gfx->setFont(&FreeMonoBold18pt7b);
      text_height = (FreeMonoBold18pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold18pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;
      break;
    case 6:  //BOLD 27 pt
      Fontname = "FreeMonoBold27pt7b";
      gfx->setFont(&FreeMonoBold27pt7b);
      text_height = (FreeMonoBold27pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold27pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;
      break;
    case 7:  //SANS BOLD 10 pt
     Fontname = "FreeSansBold10pt7b";
      gfx->setFont(&FreeSansBold10pt7b);
      text_height = (FreeSansBold10pt7bGlyphs[0x38].height) + 1;
      text_offset = -(FreeSansBold10pt7bGlyphs[0x38].yOffset);
      text_char_width = 12;
      break;
                case 8:  //SANS BOLD 18 pt
     Fontname = "FreeSansBold18pt7b";
      gfx->setFont(&FreeSansBold18pt7b);
      text_height = (FreeSansBold18pt7bGlyphs[0x38].height) + 1;
      text_offset = -(FreeSansBold18pt7bGlyphs[0x38].yOffset);
      text_char_width = 12;
      break;
            case 9:  //sans BOLD 30 pt 
      Fontname = "FreeSansBold30pt7b";
      gfx->setFont(&FreeSansBold30pt7b);
      text_height = (FreeSansBold30pt7bGlyphs[0x38].height) + 1;
      text_offset = -(FreeSansBold30pt7bGlyphs[0x38].yOffset);
      text_char_width = 12;
      break;
      case 10:  //sans BOLD 50 pt 
      Fontname = "FreeSansBold50pt7b";
      gfx->setFont(&FreeSansBold50pt7b);
      text_height = (FreeSansBold50pt7bGlyphs[0x38].height) + 1;
      text_offset = -(FreeSansBold50pt7bGlyphs[0x38].yOffset);
      text_char_width = 12;
      break;

    default:
      Fontname = "FreeMono8pt7b";
      gfx->setFont(&FreeMono8pt7b);
      text_height = (FreeMono8pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono8pt7bGlyphs[0x3D].yOffset);
      text_char_width = 12;

      break;
  }
  gfx->setTextSize(1);

}


void setup() {
  _null = 0;
  _temp = 0;
  Serial.begin(115200);
  Serial.println("Test for NMEA Display ");
  Serial.println(" IDF will throw errors here as one pin is -1!");
  ts.begin();
  ts.setRotation(ROTATION_INVERTED);

  
  #ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  #endif
  // Init Display
  gfx->begin();
  //if GFX> 1.3.1 try and do this as the invert colours write 21h or 20h to 0Dh has been lost from the structure!
  gfx->invertDisplay(false);
  gfx->fillScreen(BLACK);

  #ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
  #endif

  EEPROM_READ();  // setup and read saved variables into Saved_Settings
    setFont(5);
  gfx->setCursor(0, 100);gfx->setTextColor(WHITE);
   gfx->println(" EEPROM READ ");
  Current_Settings = Saved_Settings;
  if (Current_Settings.EpromKEY != Default_Settings.EpromKEY) {
    Current_Settings = Default_Settings;
    EEPROM_WRITE();
    Serial.println("Setting to EEPROM defaults");
  }

  connectwithsettings();

  if (WiFi.status() == WL_CONNECTED){    
    gfx->println(" Connected ! ");
    Serial.println("connected.: printing data:");}
   
  
  
  gfx->println(" Using :");
  gfx->print(WiFi.SSID());
  gfx->print(" ");
  gfx->println(WiFi.localIP());
  
  Serial.print(" *Running with:  ssid<");
  Serial.print(WiFi.SSID());Serial.print(">  Ip:"); 
  Serial.println (WiFi.localIP());
  //strcpy(Current_Settings.ssid, WiFi.SSID().c_str());  // why??
  //strcpy(Current_Settings.password, WiFi.psk().c_str());  //why??
  Udp.begin(atoi(Current_Settings.UDP_PORT));


  setFont(0);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(90, 140);
  SD_Setup();
  Audio_setup();
  keyboard(-1);  //reset keyboard display update settings
  dataline(1, Current_Settings, "Current");
  gfx->println(F("***START Screen***"));
  delay(100);  // .1 seconds
   Display(100); // trigger default 

}

void loop() {
  int unused;
  //
  //DisplayCheck(false);
  //EventTiming("START");
  ts.read();
  //TouchSample(TouchData);
  Display(Current_Settings.DisplayPage);  //EventTiming("STOP");
  TestInputsOutputs();
  //EventTiming(" loop time touch sample display");
  //Use_Keyboard(Current_Settings.password,sizeof(Current_Settings.password));
  audio.loop(); //
  vTaskDelay(1);    // Audio is distorted without this?? used in https://github.com/schreibfaul1/ESP32-audioI2S/blob/master/examples/plays%20all%20files%20in%20a%20directory/plays_all_files_in_a_directory.ino
  //.... (audio.isRunning()){   delay(100);gfx->println("Playing Ships bells"); Serial.println("Waiting for bells to finish!");}
}


void TouchCrosshair(int size) {
 for (int i = 0; i < (ts.touches); i++) {
  TouchCrosshair(i, size, WHITE);}
}
void TouchCrosshair(int point, int size, uint16_t colour) {
  gfx->setCursor(ts.points[point].x, ts.points[point].y);
  gfx->printf("%i",point);
  gfx->drawFastVLine(ts.points[point].x, ts.points[point].y - size, 2 * size, colour);
  gfx->drawFastHLine(ts.points[point].x - size, ts.points[point].y, 2 * size, colour);
}


void DisplayCheck(bool invertcheck) {
  static unsigned long timedInterval;
  static unsigned long updatetiming;
  static unsigned long LoopTime;
  static int Color;
  static bool ips;
  LoopTime = millis() - updatetiming;
  updatetiming = millis();
  if (millis() >= timedInterval) {
    //***** Timed updates *******
    timedInterval = millis() + 300;
    //Serial.printf("Loop Timing %ims",LoopTime);
    ScreenShow(1, Current_Settings, "Current");


    if (invertcheck) {
      //Serial.println(" Timed display check");
      Color = Color + 1;
      if (Color > 5) {
        Color = 0;
        ips = !ips;
        gfx->invertDisplay(ips);
      }
      gfx->fillRect(300, text_height, 180, text_height * 2, BLACK);
      gfx->setTextColor(WHITE);
      if (ips) {
        Writeat(310, text_height, "INVERTED");
      } else {
        Writeat(310, text_height, " Normal ");
      }

      switch (Color) {
        //gfx->fillRect(0, 00, 480, 20,BLACK);gfx->setTextColor(WHITE);Writeat(0,0, "WHITE");
        case 0:
          gfx->fillRect(300, 60, 180, 60, WHITE);
          gfx->setTextColor(BLACK);
          Writeat(310, 62, "WHITE");
          break;
        case 1:
          gfx->fillRect(300, 60, 180, 60, BLACK);
          ;
          gfx->setTextColor(WHITE);
          Writeat(310, 62, "BLACK");
          break;
        case 2:
          gfx->fillRect(300, 60, 180, 60, RED);
          ;
          gfx->setTextColor(BLACK);
          Writeat(310, 62, "RED");
          break;
        case 3:
          gfx->fillRect(300, 60, 180, 60, GREEN);
          ;
          gfx->setTextColor(BLACK);
          Writeat(310, 62, "GREEN");
          break;
        case 4:
          gfx->fillRect(300, 60, 180, 60, BLUE);
          ;
          gfx->setTextColor(BLACK);
          Writeat(310, 62, "BLUE");
          break;
      }
    }
  }
}






//*********** Touch stuff ****************

int swipedir(int sampleA, int sampleB, int range) {
  int tristate;
  tristate = 0;
  if (SwipedFarEnough(sampleA, sampleB, range)) {
    if ((sampleB - sampleA) > 0) {
      tristate = 1;
    } else {
      tristate = -1;
    }
  }
  return tristate;
}

bool SwipedFarEnough(int sampleA, int sampleB, int range) {  // separated so I can debug!
  int A = sampleB - sampleA;
  return (abs(A) >= range);
}

bool SwipedFarEnough(TouchMemory sampleA, TouchMemory sampleB, int range) {
  int H, V;
  H = sampleB.x - sampleA.x;
  V = sampleB.y - sampleA.y;
  //Serial.printf(" SFE [ H(%i)  V(%i),  %i ]",abs(sampleB.x - sampleA.x),abs(sampleB.y - sampleA.y),range );
  return ((abs(H) >= range) || abs(V) >= range);
}

// bool CheckButton(Button button){
//   static unsigned long LastDetect;
//   static bool keynoted;
//   ///bool XYinBox(int touchx,int touchy, int h,int v,int width,int height){
//     return (XYinBox(ts.points[0].x, ts.points[0].y,button.h,button.v,button.width,button.height) );
//   }
bool CheckButton(Button &button ){ // trigger on release. ?needs index (s) to remember which button!
  //trigger on release! does not sense !isTouched ..  use Keypressed in each button struct to keep track! 
 
  if (ts.isTouched && !button.Keypressed &&(millis()-button.LastDetect >=250)) { 
   if (XYinBox(ts.points[0].x, ts.points[0].y,button.h,button.v,button.width,button.height)) {
        //Serial.printf(" Checkbutton size%i state %i %i \n",ts.points[0].size,ts.isTouched,XYinBox(ts.points[0].x, ts.points[0].y,button.h,button.v,button.width,button.height));
          button.Keypressed=true; button.LastDetect= millis();}return false; }
  if (button.Keypressed && (millis()-button.LastDetect >=250)){
      //Serial.printf(" Checkbutton released from  %i %i\n",button.h,button.v);
        button.Keypressed=false; return true;
        }
  return false;
  }


bool IncrementInRange(int _increment, int& Variable, int Varmin, int Varmax, bool Cycle_Round) {  // use wwith swipedir (-1.0,+1)
  int _var;
  //bool altered = true;
  if (_increment == 0) { return false; }
  _var = Variable + _increment;
  if (Cycle_Round) {
    if (_var >= Varmax + 1) { _var = Varmin; }
    if (_var <= Varmin - 1) { _var = Varmax; }
  } else {  // limit at max min
    if (_var >= Varmax + 1) { _var = Varmax; }
    if (_var <= Varmin - 1) { _var = Varmin; }
  }
  Variable = _var;
  return true;
}

bool Swipe2(int& _LRvariable, int LRvarmin, int LRvarmax, bool LRCycle_Round,
            int& _UDvariable, int UDvarmin, int UDvarmax, bool UDCycle_Round, bool TopScreenOnly) {
  static TouchMemory Startingpoint, Touch_snapshot;
  static bool SwipeFound, SwipeStart, MidSwipeSampled;
  static unsigned long SwipeStartTime, SwipeFoundSent_atTime, timenow, SwipeCleared_atTime;
  int Hincrement, Vincrement;
#define minDist_Pixels 30
#define minSwipe_TestTime_ms 150

  //int Looking_at;  // horizontal/Vertical  0,1
  bool returnvalue = false;
  timenow = millis();

  if (SwipeFound && (timenow >= (SwipeFoundSent_atTime + 150))) {  // limits repetitions
    SwipeFound = false;
    MidSwipeSampled = false;  // reset so can look for new swipe
    SwipeStart = false;       //
    Serial.printf("   Timed reset after 'Found'   flags:[%i] [%i]) \n", SwipeStart, SwipeFound);
    return false;
  }

  if (SwipeStart && (timenow >= (SwipeStartTime + 1000))) {  // you have 3 sec from start to swipe. else resets start point.
    SwipeStart = false;
    MidSwipeSampled = false;
    SwipeFound = false;  // no swipe within 1500 of start - reset.
    Serial.printf("   RESET on Start timing flags [%i] [%i]) \n", SwipeStart, SwipeFound);
    return false;
  }

  if (SwipeFound) { return false; }  // stop doing anything if we have Swipe sensed already (will wait for timed reset)
  if (!ts.isTouched){ return false; } // is never triggered! 
  //************* ts must be touched to get here....*****************
  if (TopScreenOnly && (ts.points[0].y >= 180)) { return false; }
  //Capture Snapshot of touch point: in case in changes during tests
  //if (timenow <= SwipeCleared_atTime+ 200) {return false;}  // force a gap after a swipe clear. halts function!!
  if ((timenow - Touch_snapshot.sampletime) <= 20) { return false; }  // 20ms min update

  Touch_snapshot.sampletime = timenow;
  Touch_snapshot.x = ts.points[0].x;
  Touch_snapshot.y = ts.points[0].y;
  Touch_snapshot.size = ts.points[0].size;

  if (!SwipeStart) {  // capture First cross hair after reset ?
    Startingpoint = Touch_snapshot;
    // Serial.printf(" Starting at x%i y%i\n", Startingpoint.x, Startingpoint.y);
    //TouchCrosshair(10, WHITE);
    SwipeStart = true;
    SwipeFound = false;
    MidSwipeSampled = false;
    SwipeStartTime = timenow;  // so we can reset on time if we do not 'swipe'
    return false;              // we have the start point, so just return false.
  }
  //test
  //Ignore this first movement- its just to ensure swiping properly. I had sensied this, but its not consistent!
  if (SwipeStart && !MidSwipeSampled && SwipedFarEnough(Startingpoint, Touch_snapshot, minDist_Pixels)) {
    Startingpoint = Touch_snapshot;
    MidSwipeSampled = true;
    //TouchCrosshair(20, BLUE);
    return false;
  }
  if (SwipeStart && MidSwipeSampled && SwipedFarEnough(Startingpoint, Touch_snapshot, (minDist_Pixels))) {
    Hincrement = swipedir(Startingpoint.x, Touch_snapshot.x, minDist_Pixels);
    Vincrement = swipedir(Touch_snapshot.y, Startingpoint.y, minDist_Pixels);
    // only return true if we increment (change) either of the selected attributes
    // Tried evaluating seperately at mid point.. but values were NOT consistent Do H and V separately as one may not be consistent! 
   // Serial.printf(" swipe evaluate  start-mid(H%i  V%i)   mid-now(H%i V%i) ",Hincrement[0],Vincrement[0],Hincrement[1],Vincrement[1]);
   if (IncrementInRange(Hincrement, _LRvariable, LRvarmin, LRvarmax, LRCycle_Round)) {returnvalue =true;}
   if (IncrementInRange(Vincrement, _UDvariable, UDvarmin, UDvarmax, UDCycle_Round)) {returnvalue = true;}
  
    SwipeFoundSent_atTime = timenow;
    SwipeStart = false;
    MidSwipeSampled = false;
    SwipeFound = true;  // we have gone far enough to test swipe, so start again..
   // if (returnvalue){TouchCrosshair(20, GREEN);} else {TouchCrosshair(20, RED);}

  }  // SWIPEStart recorded, now looking for swipe > min distance (swipeFound)//
  return returnvalue;
}


void TouchValueShow(int offset, bool debug) {  // offset display down in pixels
  //ts.read();
  if (ts.isTouched) {
    for (int i = 0; i < (ts.touches); i++) {
      if (debug) {
        Serial.printf("Touch [%i] X:%i Y:%i size:%i \n", i + 1, ts.points[i].x, ts.points[i].y, ts.points[i].size);
        GFXBoxPrintf(0, ((i * 10) + offset), "Touch [%i] X:%i Y:%i size:%i ", i + 1, ts.points[i].x, ts.points[i].y, ts.points[i].size);
      }
    }
  }
}



//*********** EEPROM functions *********
void EEPROM_WRITE() {
  // save my current settings
  //dataline(1,Current_Settings, "EEPROM_save");
  Serial.println("SAVING EEPROM");
  EEPROM.put(0, Current_Settings);
  EEPROM.commit();
  delay(50);
}
void EEPROM_READ() {
  EEPROM.begin(512);
  Serial.println("READING EEPROM");
  EEPROM.get(0, Saved_Settings);
  dataline(1,Saved_Settings, "EEPROM_Read");
}

//************** display housekeeping ************
void ScreenShow(int LINE, MySettings A, String Text) {
  //gfx->setTextSize(1);
  GFXBoxPrintf(0, 0, 1, "%s: Seron<%s>UDPon<%s> ESPon<%s>", Text, A.Serial_on On_Off,  A.UDP_ON On_Off, A.ESP_NOW_ON On_Off);
  GFXBoxPrintf(0, text_height, 1, " SSID<%s> PWD<%s> UDPPORT<%s> ",  A.ssid, A.password,A.UDP_PORT);
}

void dataline(int line, MySettings A, String Text) {
  ScreenShow(line, A, Text);
  // Serial.printf("%d Dataline display %s: Ser<%d> UDPPORT<%d> UDP<%d>  ESP<%d> \n ", A.EpromKEY, Text, A.Serial_on, A.UDP_PORT, A.UDP_ON, A.ESP_NOW_ON);
  // Serial.print("SSID <");
  // Serial.print(A.ssid);
  // Serial.print(">  Password <");
  // Serial.print(A.password);
  // Serial.println("> ");
}
boolean CompStruct(MySettings A, MySettings B) {  // does not check ssid and password
  bool same = false;
  // have to check each variable individually
  if (A.EpromKEY == B.EpromKEY) { same = true; }
  if (A.UDP_PORT == B.UDP_PORT) { same = true; }
  if (A.UDP_ON == B.UDP_ON) { same = true; }
  if (A.ESP_NOW_ON == B.ESP_NOW_ON) { same = true; }
  if (A.Serial_on == B.Serial_on) { same = true; }
  return same;
}

void Writeat(int h, int v, int size, const char* text) {  //Write text at h,v (using fontoffset to use TOP LEFT of text convention)
  gfx->setCursor(h, v + (text_offset * size));            // offset up/down for GFXFONTS that start at Bottom left. Standard fonts start at TOP LEFT
  gfx->print(text);
  gfx->setTextSize(1);
}
void Writeat(int h, int v, const char* text) {
  Writeat(h, v, 1, text);
}
// try out void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
void WriteinBox(int h, int v, int size, const char* TEXT,uint16_t TextColor,uint16_t BackColor) {  //Write text in filled box of text height at h,v (using fontoffset to use TOP LEFT of text convention)
  int width,height;
  
  gfx->fillRect(h, v, 480, text_height * size, BackColor);
  gfx->setTextColor(TextColor);
  gfx->setTextSize(size);
  Writeat(h, v, size, TEXT);  // text offset is dealt with in write at
}
void WriteinBox(int h, int v, int size, const char* TEXT) {  //Write text in filled box of text height at h,v (using fontoffset to use TOP LEFT of text convention)
  gfx->fillRect(h, v, 480, text_height * size, WHITE);
  gfx->setTextColor(BLACK);
  gfx->setTextSize(size);
  Writeat(h, v, size, TEXT);  // text offset is dealt with in write at
}

void GFXBoxPrintf(int h, int v, int size, uint16_t TextColor,uint16_t BackColor, const char* fmt, ...) {  //complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
  static char msg[300] = { '\0' };                       // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBox(h, v, size, msg, TextColor,BackColor);
}

// void GFXBoxPrintf(int h, int v, const char* fmt, ...) {
//  GFXBoxPrintf(h, v, 1, fmt); 
// }
void GFXBoxPrintf(int h, int v, const char* fmt, ...) {  //complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
  static char msg[300] = { '\0' };                       // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBox(h, v, 1, msg);
}
void GFXBoxPrintf(int h, int v, int size, const char* fmt, ...) {  //complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
  static char msg[300] = { '\0' };                                 // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBox(h, v, size, msg);  // includes font size
}



void GFXPrintf(int h, int v, const char* fmt, ...) {  //complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
  static char msg[300] = { '\0' };                    // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  Writeat(h, v, msg);
}

// more general versions including box width size Box draws border OUTside topleft position by 'bordersize'

void WriteinBorderBox(int h, int v, int width, int height, int textsize, int bordersize, 
  uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* TEXT) {  //Write text in filled box of text height at h,v (using fontoffset to use TOP LEFT of text convention)
  int16_t x,y,TBx1,TBy1;
  uint16_t TBw,TBh;
  gfx->setTextSize(textsize);
  x=h+bordersize; y=v + bordersize+ (text_offset * textsize); //initial Top Left positioning for print text
  gfx->getTextBounds(TEXT,x,y,&TBx1,&TBy1,&TBw,&TBh); // do not forget '& ! Pointer not value!!!
  //Serial.printf(" Text bounds planned print (x%i y%i)  W:%i H:%i TB_X %i  TB_Y %i \n",x,y, TBw,TBh,TBx1,TBy1);
  //gfx->fillRect(TBx1 , TBy1 , TBw , TBh , WHITE); delay(100); // visulize what the data is!
  // move to center is  add (width-2*bordersize-TBw)/2 ?
  //move vertical is add (height -2*bordersize-TBh)/2
  gfx->fillRect(h , v , width , height , bordercol);
  gfx->fillRect(h+ bordersize, v+ bordersize, width- (2 * bordersize), height- (2 * bordersize), backgroundcol);
  gfx->setTextColor(textcol);
  
    // offset up/down by OFFSET (!) for GFXFONTS that start at Bottom left. Standard fonts start at TOP LEFT
  x=h+bordersize; y=v + bordersize+ (text_offset * textsize);
  x=x+( (width-TBw-(2*bordersize))/2)/textsize; //try horizontal centering 
  y=y+( (height-TBh-(2*bordersize))/2)/textsize;//try vertical centering
  gfx->setCursor(x, y);
  gfx->print(TEXT);
  //reset font ...
  gfx->setTextSize(1);
}

//complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
/* same as struct Button { int h, v, width, height, bordersize;
uint16_t backcol,textcol,barcol;
}; but button misses out textsize
*/
void GFXBorderBoxPrintf(Button button,const char* fmt, ...){
  static char msg[300] = { '\0' };                                                                                                                                          // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBorderBox(button.h, button.v, button.width, button.height, 1, button.bordersize, button.backcol, button.textcol, button.bordercol, msg);
}



void GFXBorderBoxPrintf(Button button,int textsize,const char* fmt, ...){
  static char msg[300] = { '\0' };                                                                                                                                          // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBorderBox(button.h, button.v, button.width, button.height, textsize, button.bordersize, button.backcol, button.textcol, button.bordercol, msg);
}
void GFXBorderBoxPrintf(int h, int v, int width, int height, int textsize, int bordersize, 
  uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
  static char msg[300] = { '\0' };                                                                                                                                          // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBorderBox(h, v, width, height, textsize, bordersize, backgroundcol, textcol, bordercol, msg);
}
void UpdateBB_DATA(int h, int v, int width, int height, int textsize, 
int bordersize, uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
  static char msg[300] = { '\0' };
  // calculate new offsets to just center on original box - minimum redraw of blank 
  int16_t x,y,TBx1,TBy1;
  uint16_t TBw,TBh;    
  gfx->setTextSize(textsize);                                                                                                                                     // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  gfx->getTextBounds(msg,h,v,&TBx1,&TBy1,&TBw,&TBh); // do not forget '& ! Pointer not value!!!
  x=h+bordersize; y=v + bordersize+ (text_offset * textsize);
  x=x+( (width-TBw-(2*bordersize))/2)/textsize; //try horizontal centering 
  y=y+( (height-TBh-(2*bordersize))/2)/textsize;//try vertical centering
  gfx->fillRect(h+ bordersize, v+ bordersize, width- (2 * bordersize), height- (2 * bordersize), backgroundcol); // full inner rectangle blanking
  gfx->setTextColor(textcol);
  gfx->setCursor(x, y);
  gfx->print(msg);
  gfx->setTextSize(1);
 // WriteinBorderBox(h, v, width, height, textsize, 0, backgroundcol, textcol, backgroundcol, msg);
}





//********* Send Advice function - useful for messages can be switched on/off here for debugging
void sendAdvice(String message) {  // just a general purpose advice send that makes sure it sends at 115200

  Serial.print(message);
}
void sendAdvicef(const char* fmt, ...) {  //complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
  static char msg[300] = { '\0' };        // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  // add checksum?
  int len = strlen(msg);
  sendAdvice(msg);
  delay(10);  // let it send!
}

//************ TIMING FUNCTIONS FOR TESTING PURPOSES ONLY ******************
//Note this is also an example of how useful Function overloading can be!!
void EventTiming(String input) {
  EventTiming(input, 1);  // 1 should be ignored as this is start or stop! but will also give immediate print!
}

void EventTiming(String input, int number) {  // Event timing, Usage START, STOP , 'Descrption text'   Number waits for the Nth call before serial.printing results (Description text + results).
  static unsigned long Start_time;
  static unsigned long timedInterval;
  static unsigned long _MaxInterval;
  static unsigned long SUMTotal;
  static int calls = 0;
  static int reads = 0;
  long NOW = micros();
  if (input == "START") {
    Start_time = NOW;
    return;
  }
  if (input == "STOP") {
    timedInterval = NOW - Start_time;
    SUMTotal = SUMTotal + timedInterval;
    if (timedInterval >= _MaxInterval) { _MaxInterval = timedInterval; }
    reads++;
    return;
  }
  calls++;
  if (calls < number) { return; }
  if (reads >= 2) {

    if (calls >= 2) {
      Serial.print("\r\n TIMING ");
      Serial.print(input);
      Serial.print(" Using (");
      Serial.print(reads);
      Serial.print(") Samples");
      Serial.print(" last: ");
      Serial.print(timedInterval);
      Serial.print("us Average : ");
      Serial.print(SUMTotal / reads);
      Serial.print("us  Max : ");
      Serial.print(_MaxInterval);
      Serial.println("uS");
    } else {
      Serial.print("\r\n TIMED ");
      Serial.print(input);
      Serial.print(" was :");
      Serial.print(timedInterval);
      Serial.println("uS");
    }
    _MaxInterval = 0;
    SUMTotal = 0;
    reads = 0;
    calls = 0;
  }
}


//SD and image functions  include #include "JpegFunc.h"
static int jpegDrawCallback(JPEGDRAW *pDraw) {
  // Serial.printf("Draw pos = %d,%d. size = %d x %d\n", pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  gfx->draw16bitBeRGBBitmap(pDraw->x, pDraw->y, pDraw->pPixels, pDraw->iWidth, pDraw->iHeight);
  return 1;
}


void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);
    gfx->printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            gfx->print("  DIR : ");gfx->println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());gfx->print("  FILE : ");gfx->println(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void SD_Setup(){
      
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if(!SD.begin(SD_CS)){
        Serial.println("Card Mount Failed");
        return;
    } else {
      unsigned long start = millis();
    jpegDraw(JPEG_FILENAME_LOGO, jpegDrawCallback, true /* useBigEndian */,
             0 /* x */, 0 /* y */, gfx->width() /* widthLimit */, gfx->height() /* heightLimit */);
    Serial.printf("Time used: %lums\n", millis() - start);

    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    // Serial.printf("SD Card Size: %lluMB\n", cardSize);
    // Serial.println("*** SD card contents  (to three levels) ***");

    //listDir(SD, "/", 3);


}

//*****   AUDIO ....


void Audio_setup(){
  Serial.println("Audio setup ");
  audio.setPinout(I2S_BCLK, I2S_LRCK, I2S_DOUT);
  audio.setVolume(15);  // 0...21
  if (audio.connecttoFS(SD, AUDIO_FILENAME_START)) {
    gfx->setCursor(10, 100);
    delay(200);
    if (audio.isRunning()){gfx->println("Playing Ships bells");}
    while (audio.isRunning()){ audio.loop(); vTaskDelay(1); }
  } else {
    gfx->setCursor(10, 100);
    gfx->print(" Failed  audio setup ");
    Serial.println("Audio setup FAILED");
  };
  gfx->println("Exiting Setup");
 // audio.connecttoFS(SD, AUDIO_FILENAME_02); // would start some music -- not needed !

}

 void ShowData(bool& Line_Ready, char* buf ) {  //&sets pointer so I can modify Line_Ready in this function
//if (Line_Ready){
  if (buf[0] != 0) {
//     WriteLine = WriteLine + 1;
//     if (WriteLine * font > (NumberoftextLines * 2)) { WriteLine = 1; }
//     int32_t ypos = 8 + (WriteLine * font * (TEXT_HEIGHT / 2));
//     Line_Ready = false;
//     gfx->setTextColor(WHITE);
//     gfx->fillRect(0, ypos, XMAX, font * (TEXT_HEIGHT / 2), TFT_BLACK);
//     gfx->drawString(buf, 0, ypos, font);
//     Line_Ready = false;
//     if (TEXT_Colour == TFT_GREEN) {
//       Serial.printf("esp_now :%s", buf);
//       buf[0] = 0;
//       return;
//     }
//     if (TEXT_Colour == TFT_BLUE) {
      Serial.printf("UDP     :%s \n", buf);
      buf[0] = 0;
      return;
//     }
//     if (TEXT_Colour == TFT_WHITE) {
//       Serial.printf("Serial  :%s", buf);
//       buf[0] = 0;
//       return;
//     }
   }
 }
void connectwithsettings() {
  uint32_t StartTime =millis();
  gfx->print(" Using EEPROM settings");
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(Current_Settings.ssid, Current_Settings.password);
  while ((WiFi.status() != WL_CONNECTED)&& (millis() <= StartTime+ 10000)) { //Give it 10 seconds 
    delay(500);
    gfx->print(".");
  }
}

void TestInputsOutputs() {
  // if (Current_Settings.ESP_NOW_ON) { ShowData(line_EXT, nmea_EXT, Current_Settings.ListTextSize, TFT_GREEN); }
  // if (Current_Settings.Serial_on) {
  //   Test_Serial_1();
  //   ShowData(line_1, nmea_1, Current_Settings.ListTextSize, TFT_WHITE);
  // }
//   if (Current_Settings.UDP_ON) {
   Test_U();
   ShowData(line_U, nmea_U);
   line_U=false;
//   }
 }

void Test_U() {  // check if udp packet  has arrived
   static int Skip_U = 1;
  if (!line_U) {  // only process if we have dealt with the last line.
    nmea_U[0] = 0x00;
    int packetSize = Udp.parsePacket();
    if (packetSize) {  // Deal with UDP packet
      if (packetSize >= (BufferLength + 4)) {
        Udp.flush();
        return;
      }  // Simply discard if too long
      int len = Udp.read(nmea_U, BufferLength);
      byte b = nmea_U[0];
      nmea_U[len] = 0;
      line_U = true;
    }  // udp PACKET DEALT WITH
 }
}

