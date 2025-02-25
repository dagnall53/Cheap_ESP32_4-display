
#include "Keyboard.h"
#include <Arduino_GFX_Library.h> // also includes Arduino etc, so variable names are understood
// Based on  for keyboard  trying to use elements of https://github.com/fbiego/esp32-touch-keyboard/tree/main
// modified to use char and not strings.
#include "fonts.h"

char TOP[] = "qQ1wW2eE3rR4tT5yY6uU7iI8oO9pP0";
char MIDDLE[] = "aA_sS/dD:fF;gG(hH)jJ$kK&lL@";
char BOTTOM[] = "^^^zZ.xX,cC?vV!bB'nN\"mM-";


bool change = false;
int sz = 3;  //Modulus of key definitions
int Keyboard_X =0;
int Keyboard_Y = 240;
int KBD_size = 2;  //generic size modifier for Kbd 1=small, 2=480 wide

  #define hoffset 5
  #define voffset 35 

extern int text_height;  //so we can get them if we change heights etc inside functions

int KEYBOARD_Y(void){
  return Keyboard_Y;
}
int KEYBOARD_X(void){
  return Keyboard_X;
}
void DrawKey(int KBD_size, int x, int rows_down, int width, String text ){
      gfx->drawRoundRect(Keyboard_X+(x*KBD_size), Keyboard_Y + (30*rows_down*KBD_size), width*KBD_size, 25*KBD_size, 3, RED);
      gfx->setCursor(Keyboard_X+(x*KBD_size)+(2*KBD_size), Keyboard_Y + (30*rows_down*KBD_size)+voffset);
      gfx->print(text);
} 

     
void keyboard(int type) {
 static int lasttype;
 int oldsize,oldheight;
 //Serial.printf(" setup keyboard %i  was%i \n",type,lasttype);
  if (type == -1){lasttype=6; return;} // silly number to reset things
  if (lasttype == type) {return;}
  Serial.printf("\n*** Start keyboard type %i  last type%i \n",type,lasttype);
  lasttype=type;
  ///gfx->setFont(&FreeMonoBold18pt7b);
  oldheight=text_height;
  gfx->setTextSize(KBD_size);
  gfx->setTextColor(WHITE);// reset in case its has been changed!
  gfx->fillRect(Keyboard_X,Keyboard_Y,480-Keyboard_X,480-Keyboard_Y,BLACK);
  for (int x = 0; x < 10; x++) {
    int a = KBD_size*((x * 4) + (20 * x) + 2) + Keyboard_X;
    gfx->drawRoundRect(a, Keyboard_Y, 20*KBD_size, 25*KBD_size, 3, WHITE);
    gfx->setCursor(a + hoffset, Keyboard_Y + voffset);
    gfx->print(TOP[((x * sz) + type)]);
  }

  for (int x = 0; x < 9; x++) {
    int a = KBD_size*((x * 4) + (20 * x) + 13) + Keyboard_X;
    gfx->drawRoundRect(a, Keyboard_Y + (30*KBD_size), 20*KBD_size, 25*KBD_size, 3, WHITE);
    gfx->setCursor(a + hoffset, Keyboard_Y + (30*KBD_size) + voffset);
    gfx->print(MIDDLE[((x * sz) + type)]);
  }

  for (int x = 0; x < 8; x++) {
    int a = KBD_size*((x * 4) + (20 * x) + 25) + Keyboard_X;
    gfx->drawRoundRect(a, Keyboard_Y + (60*KBD_size), 20*KBD_size, 25*KBD_size, 3, x == 0 ? GREEN : WHITE);
    gfx->setCursor(a + hoffset, Keyboard_Y + (60*KBD_size) + voffset);
    gfx->print(BOTTOM[((x * sz) + type)]);
  }
   ///gfx->setFont(&FreeMonoBold12pt7b);
   DrawKey(2,50, 3,30,"CLR");
   DrawKey(2,155, 3,30,"DEL");
   DrawKey(2,190, 3,50,"ENT");
   DrawKey(2,5, 3,30,"rst");
  gfx->drawRoundRect((90*KBD_size)+ Keyboard_X, Keyboard_Y + (90*KBD_size), 60*KBD_size, 25*KBD_size, 3, WHITE);
  gfx->setTextSize(oldsize);
}
extern void TouchCrosshair(int);

bool XYinBox(int touchx,int touchy, int h,int v,int width,int height){ //xy position in, xy top left width and height 
   return ((touchx >= h && touchx <= h+width) && (touchy >= v && touchy <= v+height));
}

bool XYinBox(int x, int y ,int Kx, int Krows_down, int Kwidth){ // use DrawKey type key setting
//Serial.printf(" Testing accuracy Target is ")

    int h=Keyboard_X+(Kx*KBD_size);
    int width=Kwidth*KBD_size;
    int v=Keyboard_Y + (30*Krows_down*KBD_size); //30 is key V spacing AT SIZE 1
    int height=25*KBD_size;                      //25 IS KEY HEIGHT AT SIZE 1
  return XYinBox(x,y,h,v,width,height);
}


bool KeyOver(int x, int y, char* Key, int type){ //char array version
  bool Keyfound=false;

   if ((y > Keyboard_Y) && (y < (Keyboard_Y+25*KBD_size))) { //a, Keyboard_Y, 20, 25,
    //top row  
   //Serial.printf(" In TOP ROW  KBD_size;(%i) x%i y%i   looking in space x%i y%i\n",KBD_size,x,y );
   /*  for (int x = 0; x < 10; x++) {
    int a = KBD_size*((x * 4) + (20 * x) + 2) + Keyboard_X;
    gfx->drawRoundRect(a, Keyboard_Y, 20*KBD_size, 25*KBD_size, 3, WHITE);
    gfx->setCursor(a + hoffset, Keyboard_Y + voffset);
    gfx->print(TOP[((x * sz) + type)]);
   */
    for (int z = 0; z < 10; z++) {
      int a = (KBD_size*((z * 4) + (20 * z) + 2)) + Keyboard_X;
      int b = a + (20*KBD_size);
      if (x > a && x < b) {strcpy(Key,String(TOP[(z * sz) + type]).c_str());Keyfound=true;}
    }
  }
 
  if ((y > (Keyboard_Y + (30*KBD_size))) && (y < (Keyboard_Y + (30*KBD_size)+(25*KBD_size)))) { //Keyboard_Y + (30*KBD_size), 20, 25, 1, WHITE);
    //middle row
      // Serial.printf(" In MIDDLE ROW   %i %i",x,y);
    for (int z = 0; z < 9; z++) {
      int a = KBD_size*((z * 4) + (20 * z) + 13) + Keyboard_X;
      int b = a + (20*KBD_size);
      if (x > a && x < b) {strcpy(Key,String(MIDDLE[(z * sz) + type]).c_str());Keyfound=true;}
    }
  }

  if ((y > (Keyboard_Y + (60*KBD_size))) && (y < (Keyboard_Y + (60*KBD_size)+(25*KBD_size)))) {
    //bottom row
    for (int z = 0; z < 8; z++) {
      int a = KBD_size*((z * 4) + (20 * z) + 25) + Keyboard_X;
      int b = a + (20*KBD_size);
      if (x > a && x < b) {strcpy(Key,String(BOTTOM[(z * sz) + type]).c_str());Keyfound=true;}
    }
  }
  if (XYinBox(x,y,55,3,30)){ strcpy(Key,"CLR");Keyfound=true;}
  if (XYinBox(x,y,90,3,60)){ strcpy(Key," ");Keyfound=true;}
  if (XYinBox(x,y,155,3,30)){strcpy(Key,"DEL");Keyfound=true;}
  if (XYinBox(x,y,10,3,30)){strcpy(Key,"rst");Keyfound=true;}
  
  if (XYinBox(x,y,190,3,50)){strcpy(Key,"ENT");Keyfound=true;}
 return Keyfound;
 }