 WORD CLOCK - NeoMatrix 8x8 Desktop Edition
================
 by Andy Doro & Dano Wall, chnaged to DS3231 and MetroMini by Alec Peterson
 
https://andydoro.com/wordclockdesktop/

A word clock which uses NeoPixel RGB LEDs for a color shifting effect.

For more information, follow this guide: https://learn.adafruit.com/neomatrix-8x8-word-clock/

Hardware:
-------
 
 - [Adafruit MetroMini](https://www.adafruit.com/product/2590) (should work with other Arduino-compatibles with minor modifications) 
 - [DS3231 RTC breakout](https://www.adafruit.com/product/3013) (the DS3231 is far more accurate)
 - [NeoPixel NeoMatrix 8x8](https://www.adafruit.com/products/1487)
 - laser cut faceplate & enclosure
 
Software:
-------
 
This code requires the following libraries:
 
 - [RTClib](https://github.com/adafruit/RTClib)
 - [DST_RTC](https://github.com/andydoro/DST_RTC)
 - [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
 - [Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
 - [Adafruit_NeoMatrix](https://github.com/adafruit/Adafruit_NeoMatrix)


Wiring:
-------

 - Solder DS3231 breakout to the MetroMini, A2 to GND, A3 to PWR, A4 to SDA, A5 to SCL  
 - Solder NeoMatrix 5V to MetroMini 5V, GND to GND, DIN to MetroMini pin 8.
 

grid pattern:
-------

 ```
 A T W E N T Y D
 Q U A R T E R Y
 F I V E H A L F
 D P A S T O R O
 F I V E I G H T
 S I X T H R E E
 T W E L E V E N
 F O U R N I N E
 ```
 
Acknowledgements:
  - Thanks [Dano](https://github.com/danowall) for faceplate / 3D models & project inspiration! 
 

