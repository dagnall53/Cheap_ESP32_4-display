/*******************************************************************************

From scratch build! 
 for keyboard  trying to use elements of https://github.com/fbiego/esp32-touch-keyboard/tree/main
*/
// * Start of Arduino_GFX setting

#include <Arduino_GFX_Library.h>
//select pin definitions for installed GFX version (test with 1.5.3 and 1.3.1)
#include "GUITIONESP32-S3-4848S040_GFX_131.h"

// swipe function needs steady slow movement 
/*******************************************************************************
 * End of Arduino_GFX setting  - 
 ******************************************************************************/
#include <TAMC_GT911.h>
#define TOUCH_ROTATION ROTATION_NORMAL
#define TOUCH_MAP_X1 480
#define TOUCH_MAP_X2 0
#define TOUCH_MAP_Y1 480
#define TOUCH_MAP_Y2 0

TAMC_GT911 ts = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);
#include <EEPROM.h>
#include "fonts.h"

//**   structures for my variables (for saving)
struct MySettings {  //key,default page,ssid,PW,Displaypage, UDP,Mode,serial,Espnow,Listtext size
  int EpromKEY;      // to allow check for clean EEprom and no data stored change in the default will result in eeprom being reset
  int DisplayPage;   // start page after defaults (currently -100 to show fonts data)
  char ssid[16];
  char password[16];
  int UDP_PORT;
  bool UDP_ON;
  bool Serial_on;
  bool ESP_NOW_ON;
};
// use page -100 for any swipe testing 
// change key (first parameter) to set defaults
MySettings Default_Settings = { 2, 0, "N2K0183-proto", "12345678", 2002, false, true, true };
MySettings Saved_Settings;
MySettings Current_Settings;
struct Displaysettings {
  bool keyboard, CurrentSettings;
  bool NmeaDepth, nmeawind, nmeaspeed, GPS;
};
Displaysettings Display_default = { false, true, false, false, false, false };
Displaysettings Display_Setting;

struct BarChart {
  int topleftx, toplefty, bottomrightx, bottomrighty, border, background, barcolour, value, rangemin, rangemax, visible;
};


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



//*********** for keyboard*************
#include "Keyboard.h"

int caps = 0;

int text_height = 12;      //so we can get them if we change heights etc inside functions
int text_offset = 12;      //offset is not equal to height, as subscripts print lower than 'height'
int text_char_width = 12;  // useful for monotype? only NOT USED YET! Try tft.getTextBounds(string, x, y, &x1, &y1, &w, &h);

void setFont(int font) {
  /* Options: and max height and offset attribute of GFXGlyphs.
//see Glyph for "/"! 
 FreeMono8pt7b   10 and -15
 FreeMono12pt7b  14 -15  
 FreeMono18pt7b  21 -22
 FreeMonoBold12pt7b  14, -16
 FreeMonoBold18pt7b  21, -23
 FreeSansBold10pt7b  24
 FreeSansBold18pt7b  42
 FreeSansBold24pt7b  56


*/

  switch (font) {
    // set the heights and offset to print [ in boxes. Heights in pixels are NOT the point heights!
    case 0:  // SMALL 8pt
      gfx->setFont(&FreeMono8pt7b);
      text_height = (FreeMono8pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono8pt7bGlyphs[0x3D].yOffset);
      //    text_height = 13; // note height =13 {9+4} +for superscript,  for subscripts  on pt to allow for subscripts?
      //    text_offset = 10;// start print  down from top left
      //text_char_width =  FreeMono8pt7bGlyphs[0x3D].width
      text_char_width = 12;
      break;
    case 1:  // standard 12pt
      gfx->setFont(&FreeMono12pt7b);
      text_height = (FreeMono12pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono12pt7bGlyphs[0x3D].yOffset);
      // text_height=19;
      // text_offset = 15;
      text_char_width = 12;

      break;
    case 2:  //standard 18pt
      gfx->setFont(&FreeMono18pt7b);
      text_height = (FreeMono18pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMono18pt7bGlyphs[0x3D].yOffset);
      //text_height=26;
      //text_offset = 22;
      text_char_width = 12;

      break;
    case 3:  //BOLD 12pt
      gfx->setFont(&FreeMonoBold12pt7b);
      text_height = (FreeMonoBold12pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold12pt7bGlyphs[0x3D].yOffset);
      // text_height=22;
      // text_offset = 16;
      text_char_width = 12;

      break;
    case 4:  //BOLD 18 pt
      gfx->setFont(&FreeMonoBold18pt7b);
      text_height = (FreeMonoBold18pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeMonoBold18pt7bGlyphs[0x3D].yOffset);
      //text_height=31;
      //text_offset = 23;
      text_char_width = 12;

      break;
    case 5:  //Sans  12 pt
      gfx->setFont(&FreeSansBold10pt7b);
      //test
      text_height = (FreeSansBold10pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeSansBold10pt7bGlyphs[0x3D].yOffset);
      // text_height=22;  //"( height+1
      // text_offset = 16; //'/
      text_char_width = 12;  // not used yet

      break;
    case 6:  //Sans  18 pt
      gfx->setFont(&FreeSansBold18pt7b);
      //test FreeMono8pt7bGlyphs[0x3D].height, FreeMono8pt7bGlyphs[0x3D].yOffset
      text_height = (FreeSansBold18pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeSansBold18pt7bGlyphs[0x3D].yOffset);
      //  text_height=34;  //max height in glyph [ +1
      //  text_offset = 24; //  Abs(yoffset) of /
      text_char_width = 12;  // not used yet
      break;
    case 7:  //Sans  24 pt
      gfx->setFont(&FreeSansBold24pt7b);
      text_height = (FreeSansBold24pt7bGlyphs[0x3D].height) + 1;
      text_offset = -(FreeSansBold24pt7bGlyphs[0x3D].yOffset);
      // text_height=44;  //Based on ] character max height in glyph +1
      // text_offset = 33; //  Abs(yoffset) of
      text_char_width = 12;  // not used yet
      break;
    default:
      gfx->setFont(&FreeMono8pt7b);
      text_height = 13;
      text_offset = 10;
      text_char_width = 12;

      break;
  }
  gfx->setTextSize(1);
  // text_height = text_height;
  // text_offset = text_offset;
  // text_char_width = text_char_width;
}


void setup() {
  _null = 0;
  _temp = 0;
  Serial.begin(115200);
  ts.begin();
  ts.setRotation(ROTATION_INVERTED);

  Serial.println("Test of 4848 ST7701 drivers ");
#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
  // Init Display
  gfx->begin();
  //if GFX> 1.3.1 try and do this as the invert colours write 21h or 20h to 0Dh has been lost from the structure!
  //gfx->invertDisplay(false);
  gfx->fillScreen(BLACK);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  EEPROM_READ();  // setup and read saved variables into Saved_Settings

  Current_Settings = Saved_Settings;
  if (Current_Settings.EpromKEY != Default_Settings.EpromKEY) {
    Current_Settings = Default_Settings;
    EEPROM_WRITE();
  }

  setFont(0);

  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(90, 140);

  keyboard(-1);  //reset keyboard display update settings
  dataline(1, Current_Settings, "Current");
  gfx->println(F("***START Screen***"));
  delay(500);  // .5 seconds
}

void loop() {
  int unused;
  static int swipe;
  //DisplayCheck(false);
  //EventTiming("START");
  ts.read();
  //TouchSample(TouchData);
  Display(Current_Settings.DisplayPage);  //EventTiming("STOP");
  //EventTiming(" loop time touch sample display");
  //Use_Keyboard(Current_Settings.password,sizeof(Current_Settings.password));
}


void TouchCrosshair(int size) {
  TouchCrosshair(size, WHITE);
}
void TouchCrosshair(int size, uint16_t colour) {
  gfx->drawFastVLine(ts.points[0].x, ts.points[0].y - size, 2 * size, colour);
  gfx->drawFastHLine(ts.points[0].x - size, ts.points[0].y, 2 * size, colour);
}


void Display(int page) {
  static int LastPageselected;
  static int font;
  static int SwipeTestLR, SwipeTestUD;
  static bool RunSetup;
  static unsigned int slowdown;

  if (page != LastPageselected) { RunSetup = true; }

  switch (page) {
    case -100:  //a test fonts screen
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE);
        font = 0;
        SwipeTestLR = 0;
        SwipeTestUD = 0;
        setFont(font);
        GFXBoxPrintf(0, 250, 3, "-Top page- SETUP- ");
      }

      if (millis() >= slowdown + 10000) {
        slowdown = millis();
        // font = font + 1;
        gfx->fillScreen(BLACK);
        GFXBoxPrintf(0, 50, 2, "Periodic blank ");
      }

      if (Swipe2(SwipeTestLR, 1, 10, true, SwipeTestUD, 0, 10, false, false)) {
        Serial.printf(" LR updated %i UD updated %i \n", SwipeTestLR, SwipeTestUD);  // swipe page using (current) TouchData information with LR (true) swipe
        GFXBoxPrintf(0, 0, 1, "SwipeTestLR (%i)", SwipeTestLR);
        GFXBoxPrintf(0, 480 - text_height, 1, "SwipeTestUD (%i)", SwipeTestUD);
      };

      break;
    case -1:
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE, BLACK);
        gfx->setTextSize(1);
        gfx->setCursor(180, 180);
      }
      setFont(2);
      dataline(1, Current_Settings, "Current");
      gfx->println(F("SETTINGS PAGEs -  swipe 2,4 "));
      setFont(0);
      // Swipe (TouchData, Current_Settings.DisplayPage, 2, 4, true, true);  // swipe page using (current) TouchData information with LR (true) swipe

      break;
    case 0:
      if (RunSetup) {
        gfx->fillScreen(BLACK);
        gfx->setTextColor(WHITE);
        setFont(0);
        GFXBoxPrintf(0, 50, 3, "-Top page-");
      }
      if (millis() > slowdown + 10000) {
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
      }
      // Swipe(TouchData, Current_Settings.DisplayPage, 0, 4, true, true);  // swipe page using (current) TouchData information with LR (true) swipe
      if (ts.isTouched) {
        TouchCrosshair(20);
      }

      break;
    case 1:
      if (RunSetup) {
        gfx->fillScreen(BLUE);
        gfx->setTextColor(WHITE);
        setFont(0);
        GFXBoxPrintf(0, 140, 2, "P1 Testing Set Password");
        keyboard(-1);  //reset
      }
      if (millis() > slowdown + 1000) {
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
      }

      //setup keyboard if not showing
      keyboard(caps);
      Use_Keyboard(Current_Settings.password, sizeof(Current_Settings.password));
      break;
    case 2:
      if (RunSetup) {
        gfx->fillScreen(BLUE);
        gfx->setTextColor(WHITE);
        GFXBoxPrintf(0, 140, 2, "P2 Set ssid");
        keyboard(-1);  //reset
      }
      if (millis() > slowdown + 1000) {
        slowdown = millis();
        dataline(1, Current_Settings, "Current");
      }


      keyboard(caps);
      Use_Keyboard(Current_Settings.ssid, sizeof(Current_Settings.ssid));
      break;

    default:
      if (RunSetup) { gfx->fillScreen(BLACK); }

      gfx->setCursor(180, 180);
      GFXBoxPrintf(50, 140, 2, "* Page %i", page);
      break;
  }
  LastPageselected = page;
  RunSetup = false;
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
        //gfx->invertDisplay(ips);}
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

void Use_Keyboard(char* DATA, int sizeof_data) {
  static unsigned long lastkeypressed, last_Displayed;
  static bool KeyPressUsed;
  static bool KeyCommand;
  static bool VariableChanged = false;
  char KEY[6];
  static char Local_var[30];

  int result_positionX, result_positionY;
  result_positionX = KEYBOARD_X();
  result_positionY = KEYBOARD_Y() - (3 * text_height) - 5;

  if ((!VariableChanged) || (millis() > last_Displayed)) {
    strcpy(Local_var, DATA);
    WriteinBox(result_positionX, result_positionY, 2, Local_var);
    VariableChanged = true;
    last_Displayed = millis() + 500;
  }
  int st;
  if ((ts.isTouched)) {
    st = KeyOver(ts.points[0].x, ts.points[0].y, KEY, caps);
    //Serial.printf(" Pressure test %i  KEYchr<%i> Bool <%i>\n",ts.points[0].size,ts.points[0].x, ts.points[0].y,KEY,st );
  }

  if (!KeyPressUsed && (ts.isTouched) && (ts.points[0].size > 35) && (KeyOver(ts.points[0].x, ts.points[0].y, KEY, caps))) {
    Serial.printf(" Keyboard check inputsizeof<%i>   sizeof_here *data(%i)   currentlen<%i>\n", sizeof_data, sizeof(*DATA), strlen(Local_var));
    KeyPressUsed = true;
    lastkeypressed = millis();
    KeyCommand = false;
    //Serial.printf(" Key test %s \n",KEY);
    if (!strcmp(KEY, "^")) {
      caps = caps + 1;
      if (caps > 2) { caps = 0; }  //NB strcmp returns 0 if NO Difference, else position of non match characters
                                   ///gfx->setFont(&FreeMonoBold18pt7b);
                                   //text_height=18; //(see the name!!)
      //font_offset = text_height - 2;  // lift slightly?
      keyboard(caps);
      ///gfx->setFont(&FreeMono8pt7b);
      //text_height=8; //(see the name!!)
      //font_offset = text_height - 2;  // lift slightly?
      KeyCommand = true;
    }
    if (!strcmp(KEY, "DEL")) {
      Local_var[strlen(Local_var) - 1] = '\0';  // NOTE single inverted comma !!!
      KeyCommand = true;
    }
    if (!strcmp(KEY, "CLR")) {
      Local_var[0] = '\0';
      KeyCommand = true;
    }
    if (!strcmp(KEY, "rst")) {
      strcpy(Local_var, DATA);
      WriteinBox(result_positionX, result_positionY, 2, Local_var);
      KeyCommand = true;
    }
    if (!strcmp(KEY, "ENT")) {
      strcpy(DATA, Local_var);
      Serial.printf("Updated was %s is  %s", DATA, Local_var);
      strncpy(DATA, Local_var, sizeof_data);  // limit_size so we cannot overwrite the original array size
      KeyCommand = true;
      VariableChanged = false;
      EEPROM_WRITE;
      //Current_Settings.DisplayPage=0; //Reset to page 0
    }
    if (!KeyCommand) {  //Serial.printf(" adding %s on end of variable<%s>\n",KEY,Local_var);
      strcat(Local_var, KEY);
    }
    WriteinBox(result_positionX, result_positionY, 2, Local_var);
  }
  if (!ts.isTouched && KeyPressUsed && (millis() > (250 + lastkeypressed))) { KeyPressUsed = false; }
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
  if (!ts.isTouched){ return false; }
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
  //dataline(1,Saved_Settings, "EEPROM_Read");
}

//************** display housekeeping ************
void ScreenShow(int LINE, MySettings A, String Text) {
  //gfx->setTextSize(1);
  GFXBoxPrintf(0, 0, 1, "%s: Ser<%d> UDP<%d> UDP<%d> ESP<%d>", Text, A.Serial_on, A.UDP_PORT, A.UDP_ON, A.ESP_NOW_ON);
  GFXBoxPrintf(0, text_height, 1, "Page[%i] SSID<%s> PWD<%s>", A.DisplayPage, A.ssid, A.password);
}

void dataline(int line, MySettings A, String Text) {
  ScreenShow(line, A, Text);
  Serial.printf("%d Dataline display %s: Ser<%d> UDPPORT<%d> UDP<%d>  ESP<%d> \n ", A.EpromKEY, Text, A.Serial_on, A.UDP_PORT, A.UDP_ON, A.ESP_NOW_ON);
  Serial.print("SSID <");
  Serial.print(A.ssid);
  Serial.print(">  Password <");
  Serial.print(A.password);
  Serial.println("> ");
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
  gfx->println(text);
  gfx->setTextSize(1);
}
void Writeat(int h, int v, const char* text) {
  Writeat(h, v, 1, text);
}

void WriteinBox(int h, int v, int size, const char* TEXT) {  //Write text in filled box of text height at h,v (using fontoffset to use TOP LEFT of text convention)
  gfx->fillRect(h, v, 480, text_height * size, WHITE);
  gfx->setTextColor(BLACK);
  gfx->setTextSize(size);
  Writeat(h, v, size, TEXT);  // text offset is dealt with in write at
}

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
  WriteinBox(h, v, size, msg);  // includes size
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

void WriteinBox(int h, int v, int width, int height, int textsize, int bordersize, uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* TEXT) {  //Write text in filled box of text height at h,v (using fontoffset to use TOP LEFT of text convention)
  gfx->fillRect(h - bordersize, v - bordersize, width + (2 * bordersize), height + (2 * bordersize), bordercol);
  gfx->fillRect(h, v, width, height, backgroundcol);
  gfx->setTextColor(textcol);
  gfx->setTextSize(textsize);
  gfx->setCursor(h, v + (text_offset * textsize));  // offset up/down by OFFSET (!) for GFXFONTS that start at Bottom left. Standard fonts start at TOP LEFT
  gfx->println(TEXT);
  //reset font ...
  gfx->setTextSize(1);
}
//complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
void GFXBoxPrintf(int h, int v, int width, int height, int textsize, int bordersize, uint16_t backgroundcol, uint16_t textcol, uint16_t bordercol, const char* fmt, ...) {  //Print in a box.(h,v,width,height,textsize,bordersize,backgroundcol,textcol,bordercol, const char* fmt, ...)
  static char msg[300] = { '\0' };                                                                                                                                          // used in message buildup
  va_list args;
  va_start(args, fmt);
  vsnprintf(msg, 128, fmt, args);
  va_end(args);
  int len = strlen(msg);
  WriteinBox(h, v, width, height, textsize, bordersize, backgroundcol, textcol, bordercol, msg);
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
