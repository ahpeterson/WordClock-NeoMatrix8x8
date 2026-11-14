// One-off RTC utility for the word clock. Prints the DS3231 time every second.
// Send "T<epoch>\n" over serial (9600) to set it, where <epoch> is seconds since
// 1970 in local *standard* time (the word clock adds the DST hour itself).
// Resets never change the time, so it is safe to open the serial port.
#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 RTC;

void setup() {
  Serial.begin(9600);
  pinMode(A2, OUTPUT); digitalWrite(A2, LOW);   // RTC GND
  pinMode(A3, OUTPUT); digitalWrite(A3, HIGH);  // RTC power
  delay(50);
  Wire.begin();
  RTC.begin();
  Serial.print("lostPower="); Serial.println(RTC.lostPower());
}

void loop() {
  if (Serial.available() && Serial.read() == 'T') {
    uint32_t e = Serial.parseInt();
    if (e > 1700000000UL) {
      Serial.print("Before: "); Serial.println(RTC.now().timestamp());
      RTC.adjust(DateTime(e));
      Serial.print("Set to: "); Serial.println(RTC.now().timestamp());
    }
  }
  static uint32_t last = 0;
  if (millis() - last >= 1000) {
    last = millis();
    Serial.print("RTC: "); Serial.println(RTC.now().timestamp());
  }
}
