This project is an initial record of trying to get a cheap 4 inch square touch display with esp32 working.

search Ali express for typicaly  ESP32-S3 4.0 Inch Smart Display for Arduino LVGL WiFi Bluetooth Development Board 86 Box Central Control Panel LCD TFT Module

The display is sold as a Mains panel with one  or two relays, and is usually used for HomeAssistant.
The display is made by guitron, and demo code is provided. But I had issues trying to get mine to work, mainly as I wished to use standard libraries, and not thoise supplied with the module. examples.


LIBRARIES: 
The main issue with the module is that the GFX for Arduino Library MUSt be version 1.3.1 to use the example Configuration.
After this, the GFX Library changed how it takes in configuration data. 
In later versions, the gfx definition has removed the invert colour and rotation settings!
Unfortunately the "default" initialisation for the display after V1.3.1 sets the colours inverted: 
and then, to compound the problem, the later GFX libraries for the used ST7701 display have also unaccountably removed the gfx->invertDisplay(ips); command for that display type.
(It is still configured for other displays!!.) 
I understand that it is expected that variants of the initialisation (eg" st7701_type1_init_operations") are expected to be used to select the correct colour settings and rotation, but I have not to date managed to test this. 

This code is using structures to provide the following:

Example of saving and reading variables.
Multiple different 'display pages
A keyboard that can alter any variable
a swipe function is now working 

