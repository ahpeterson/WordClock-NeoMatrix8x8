/*
   An update of 
   WORD CLOCK - 8x8 NeoPixel Desktop Edition
   by Andy Doro

   A word clock using NeoPixel RGB LEDs for a color shift effect.
   Updates: Metro Mini in place of discontinued Trinket 5v
   higher accuracy RTC to eliminate time drift (oritginal RTC very temp. sensitive)

   Hardware:
   - Metro Mini - Adafruit #2590 (should work with other Arduino-compatibles with minor modifications)
   - DS3231 RTC breakout
   - NeoPixel NeoMatrix 8x8


   Software:

   This code requires the following libraries:

   - RTClib https://github.com/adafruit/RTClib
   - DST_RTC https://github.com/andydoro/DST_RTC
   - Adafruit_GFX https://github.com/adafruit/Adafruit-GFX-Library
   - Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
   - Adafruit_NeoMatrix https://github.com/adafruit/Adafruit_NeoMatrix


   Wiring:
   For connecting the DS3231 to the Metro Mini

   VIN - A3
   GND - A2
   SDA - A4
   SCL - A5

   For connecting the NeoMatrix 8x8 to the Metro-mini
   5V -  Mm 5V
   GND - Mm GND
   DIN - Mm 8

   

   - Solder DS_3231 breakout to MetroMini, A2 to GND, A3 to Vin, A4 to SDA, A5 to SCL

   - Solder NeoMatrix 5V to MetroMini 5V, GND to GND, DIN to MetroMini pin 8.


   grid pattern

    A T W E N T Y D
    Q U A R T E R Y
    F I V E H A L F
    D P A S T O R O
    F I V E I G H T
    S I X T H R E E
    T W E L E V E N
    F O U R N I N E


    Acknowledgements:
    - Thanks Dano for faceplate / 3D models & project inspiration!

*/

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <DST_RTC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// define how to write each of the words

// 64-bit "mask" for each pixel in the matrix- is it on or off?
uint64_t mask;

// define masks for each word. we add them with "bitwise or" to generate a mask for the entire "phrase".
#define MFIVE    mask |= 0xF00000000000        // these are in hexadecimal
#define MTEN     mask |= 0x5800000000000000
#define AQUARTER mask |= 0x80FE000000000000
#define TWENTY   mask |= 0x7E00000000000000
#define HALF     mask |= 0xF0000000000
#define PAST     mask |= 0x7800000000
#define TO       mask |= 0xC00000000
#define ONE      mask |= 0x43
#define TWO      mask |= 0xC040
#define THREE    mask |= 0x1F0000
#define FOUR     mask |= 0xF0
#define FIVE     mask |= 0xF0000000
#define SIX      mask |= 0xE00000
#define SEVEN    mask |= 0x800F00
#define EIGHT    mask |= 0x1F000000
#define NINE     mask |= 0xF
#define TEN      mask |= 0x1010100
#define ELEVEN   mask |= 0x3F00
#define TWELVE   mask |= 0xF600
#define ANDYDORO mask |= 0x8901008700000000

// define pins
#define NEOPIN 8  // connect to DIN on NeoMatrix 8x8
#define RTCGND A2 // use this as DS3231 breakout ground (GND)
#define RTCPWR A3 // use this as DS3231 breakout power (Vin)


// brightness based on time of day- could try warmer colors at night?
#define DAYBRIGHTNESS 40
#define NIGHTBRIGHTNESS 20

// cutoff times for day / night brightness. feel free to modify.
#define MORNINGCUTOFF 7  // when does daybrightness begin?   7am
#define NIGHTCUTOFF   22 // when does nightbrightness begin? 10pm


// define delays
#define FLASHDELAY 250  // delay for startup "flashWords" sequence
#define SHIFTDELAY 100   // controls color shifting speed

// monochrome mode?  Here are some colors to choose
#define WHITE 200, 255, 255
#define GREEN 0,255,0 
#define RED 255, 0, 0
#define BLUE 0, 0, 255
#define TEAL 0, 128, 128
#define VIOLET 127, 0, 255

RTC_DS3231 RTC; // Establish clock object
DST_RTC dst_rtc; // DST object

DateTime theTime; // Holds current clock time

int j;   // an integer for the color shifting effect

// Do you live in a country or territory that observes Daylight Saving Time?
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// Use 1 if you observe DST, 0 if you don't. This is programmed for DST in the US / Canada. If your territory's DST operates differently,
// you'll need to modify the code in the calcTheTime() function to make this work properly.
#define OBSERVE_DST 1


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel matrix = Adafruit_NeoPixel(64, NEOPIN, NEO_GRB + NEO_KHZ800);

// configure for 8x8 neopixel matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:

  //Serial for debugging
  Serial.begin(9600);

  Serial.println("Starting setup");
  // set pinmodes
  pinMode(NEOPIN, OUTPUT);

  // set analog pins to power DS_3231 breakout!
  pinMode(RTCGND, OUTPUT); // analog 2
  pinMode(RTCPWR, OUTPUT); // analog 3

  // set them going!
  digitalWrite(RTCGND, LOW);  // GND for RTC
  digitalWrite(RTCPWR, HIGH); // VIN for RTC

  // start clock
  Serial.println("Starting RTC");
  RTC.begin();   // begin clock
  Serial.println("Started RTC");

  if (RTC.lostPower()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
    // DST? If we're in it, let's subtract an hour from the RTC time to keep our DST calculation correct. This gives us
    // Standard Time which our DST check will add an hour back to if we're in DST.
    DateTime standardTime = RTC.now();
    if (dst_rtc.checkDST(standardTime) == true) { // check whether we're in DST right now. If we are, subtract an hour.
      standardTime = standardTime.unixtime() - 3600;
    }
    RTC.adjust(standardTime);
  }
  else {
    Serial.println("RTC running, using existing time.");
  }


  matrix.begin();
  matrix.setBrightness(DAYBRIGHTNESS);
  matrix.fillScreen(0); // Initialize all pixels to 'off'
  matrix.show();

  // startup sequence... do colorwipe?
  // delay(500);
  // rainbowCycle(20);
  delay(500);
  flashWords(); // briefly flash each word in sequence
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:

  // get the time
  theTime = dst_rtc.calculateTime(RTC.now()); // takes into account DST
  // add 2.5 minutes to get better estimates
  theTime = theTime.unixtime() + 150;

  adjustBrightness();
  displayTime();

  //mode_moon(); // uncomment to show moon mode instead.  Also be sure to comment "displayTime"


}
