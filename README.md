This project is an initial record of trying to get a cheap 4 inch square touch display with esp32 working.


This code is using structures to provide the following:

Example of saving and reading variables.
Multiple different 'display pages
A keyboard that can alter any variable
a swipe function is now working 
Now SD and Audio.

I am building in functions to help keyboard and make buttons easy to use in displays:
Structure 'button' defines position and size, and 
  works with GFX a button 'print' and a keypressed (XYinbox) function to make page setting up easy. 

At this point these functions are a bit random and duplicated! and need tidying up.

I plan to move general functions to Keyboard (or similar) to keep the main ino clean.
There is a Display(page) that selects multiple [pages] 


*************** HARDWARE ***********
search Ali express for typicaly  ESP32-S3 4.0 Inch Smart Display for Arduino LVGL WiFi Bluetooth Development Board 86 Box Central Control Panel LCD TFT Module

The display is sold as a Mains panel with one  or two relays, and is usually used for HomeAssistant.
The display is made by guitron, and demo code is provided. 
But I had issues trying to get mine to work, mainly as I wished to use standard libraries, and not those 
supplied with the module. examples.


 
LIBRARIES: 
The main issue with examples wass that the GFX for Arduino Library needed to be version 1.3.1 to use the example Configuration.
After this, the GFX Library changed how it takes in configuration data. 
In later versions, the gfx definition has removed the invert colour and rotation settings!
Unfortunately the "default" initialisation for the display after V1.3.1 sets the colours inverted: 
and then, to compound the problem, the later GFX libraries for the used ST7701 display have also unaccountably removed the gfx->invertDisplay(ips); command for that display type.
(It is still configured for other displays!!.) 
I understand that it is expected that variants of the initialisation (eg" st7701_type1_init_operations") are expected to be used to select the correct colour settings and rotation, but I have not to date managed to test this. 
I am now GFX 1.5.3 compliant - provided you take 1.53 from the git directly. The arduino 'update' seems to install a version that is not compiant with pre V3 esp32 coding.

s


SD seemed problematic  
But it was just a bad SD card. or that it is very selective about which cards work
I only have one 4gb card that actually works! 
Also board is set up to connect to relays and not the audio device!, so de and re soldering of zero ohm resistsors is required!

I think its possible to use the Makerfabs examples with GFX 1.5.3 providing you reference the latest esp32_4inch.x from here and not their GFX setups 

