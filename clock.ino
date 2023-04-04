#include <TimeLib.h>
#include <Wire.h>
#include <TinyGPSPlus.h>
#include <NeoSWSerial.h>
#include "grove_alphanumeric_display.h"
NeoSWSerial ss(2, 3);
TinyGPSPlus gps;
Seeed_Digital_Tube digits1through4;
Seeed_Digital_Tube digits5through8;
const int offset = -6;   // Central European Time
time_t prevDisplay = 0;  // when the digital clock was displayed
void setup() {
  Wire.begin();
  digits1through4.setTubeType(TYPE_4, 0x71);
  digits1through4.setBrightness(15);
  digits1through4.setBlinkRate(BLINK_OFF);
  digits1through4.fulDisplay();
  digits5through8.setTubeType(TYPE_4, 0x75);
  digits5through8.setBrightness(15);
  digits5through8.setBlinkRate(BLINK_OFF);
  digits5through8.fulDisplay();
  ss.begin(9600);
}

void loop() {
  while (ss.available()) {
    if (gps.encode(ss.read())) {
      uint32_t age = gps.time.age();
      if (age < 500) {
        tmElements_t tm;
        tm.Hour = gps.time.hour();
        tm.Minute = gps.time.minute();
        tm.Second = gps.time.second();
        tm.Day = gps.date.day();
        tm.Month = gps.date.month();
        tm.Year = gps.date.year();

        setTime(
          gps.time.hour(),
          gps.time.minute(),
          gps.time.second(),
          gps.date.day(),
          gps.date.month(),
          gps.date.year());
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) {  //update the display only if the time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
  }
}

void digitalClockDisplay() {
  int h = hour();
  if (h > 10){
    digits1through4.setTubeSingleNum(THIRD_TUBE, h / 10);
  } else{
    digits1through4.setTubeSingleNum(THIRD_TUBE, 0);
  }
  digits1through4.setTubeSingleNum(FOURTH_TUBE, h % 10);

  int m = minute();
  if (m > 10) {
    digits5through8.setTubeSingleNum(FIRST_TUBE, m / 10);
  } else{
    digits5through8.setTubeSingleChar(FIRST_TUBE, 0);
  }
  digits5through8.setTubeSingleNum(SECOND_TUBE, m % 10);
  int s = second();
  if (s > 10){
    digits5through8.setTubeSingleNum(THIRD_TUBE, s / 10);
  } else{
    digits5through8.setTubeSingleNum(THIRD_TUBE, 0);
  }
  digits5through8.setTubeSingleNum(FOURTH_TUBE, s % 10);

  digits1through4.display();
  digits5through8.display();
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}
