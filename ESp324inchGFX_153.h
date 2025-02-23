/*******************************************************************************
Pins and defines for GFX - various versions!
 ******************************************************************************/
 
#ifndef _ESPGFDEF_H_
#define _ESPGFDEF_H_
//****  Later  GFX VERSIONS HAVE Refactored  *BUS config and miss the ips (colour inversion??) setup  
#define GFX_BL 38
Arduino_DataBus *bus = new Arduino_SWSPI(
  GFX_NOT_DEFINED /* DC */,
  39 /* CS */,                // Chip Select pin
  48 /* SCK */,               // Clock pin
  47 /* SDA ? */,             // Master Out Slave In pin
  GFX_NOT_DEFINED /* MISO */  // Master In Slave Out pin (not used)
);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(  // MY BOARD modified pin numbers
  18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */,
  11 /* R0 */, 12 /* R1 */, 13 /* R2 */, 14 /* R3 */, 0 /* R4 */,
  8 /* G0/P22 */, 20 /* G1/P23 */, 3 /* G2/P24 */, 46 /* G3/P25 */, 9 /* G4/P26 */, 10 /* G5 */,
  4 /* B0 */, 5 /* B1 */, 6 /* B2 */, 7 /* B3 */, 15 /* B4 */ ,
  1 /* hsync_polarity */,      // Horizontal sync polarity
  10 /* hsync_front_porch */,  // Horizontal front porch duration
  8 /* hsync_pulse_width */,   // Horizontal pulse width
  50 /* hsync_back_porch */,   // Horizontal back porch duration
  1 /* vsync_polarity */,      // Vertical sync polarity
  10 /* vsync_front_porch */,  // Vertical front porch duration
  8 /* vsync_pulse_width */,   // Vertical pulse width
  20 /* vsync_back_porch */    // Vertical back porch duration
                               // ,1, 30000000 // Uncomment for additional parameters if needed
);
// Initialize ST7701 display // comments at end of  https://github.com/Makerfabs/ESP32-S3-Parallel-TFT-with-Touch-4inch
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  480 /* width */,  480 /* height */,  rgbpanel,
  0 /* rotation */,  true /* auto_flush */,  bus, // as defined in Arduino_DataBus *bus 
  GFX_NOT_DEFINED /* RST */,  st7701_type1_init_operations,  sizeof(st7701_type1_init_operations));
/*
 tests WITH gfx (not all tested on each version!!)
 1.3.8(all) 
 1.4.3(type 1)  and Changed line 317 of arduino_rgb_display.h to  WRITE_COMMAND_8, 0x20,   // 0x20 normal, 0x21 IPS corrects colours! 
 1.5.3(type 1 ) Change line 511 to 'normal'  WRITE_COMMAND_8, 0x20,   // 0x20 normal, 0x21 IPS
 
 static const uint8_t st7701_type   
  type 1 is Inverted colours Standard rotation (ORIGINAL SELECTS 0X60 rgb666 IN LINE 316! )
  type 2 is Inverted Colours standard rotation, very bright white washed out colours
  type 3 is NOT working ! just green with horizontal black line bars
  type 4   as 3 but orange (some flashing noticed when my display colours change)
  type 5   as 3
  type 6   as 3 black and white
  type 7   as 3 blue
  type 8  !! CORRECT COLOURS but some sort of artifact at bottom row (rollover from top?)
V1.5.3 adds type 9...


*/
//** OTHER PINS

#define TFT_BL GFX_BL
#define I2C_SDA_PIN 19     //19
#define I2C_SCL_PIN 45     //45


#define SD_SCK  48                //48
#define SD_MISO 41                //41
#define SD_MOSI 47                //47
#define SD_CS   42                //42

#define I2S_DOUT      40       //40
#define I2S_BCLK      1       //1
#define I2S_LRCK      2       //2  



#define TOUCH_INT -1          //-1
#define TOUCH_RST 38          // -1 (just uses power off?)
#define TOUCH_SDA  19
#define TOUCH_SCL  45
#define TOUCH_WIDTH  480
#define TOUCH_HEIGHT 480


// workss with (https://github.com/moononournation/Arduino_GFX) GFX 1.3.1 where:









#endif // _ESPGFDEF_H_

