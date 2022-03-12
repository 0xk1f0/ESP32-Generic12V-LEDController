#include <Arduino.h>
#include <web.h>
#include <rgb.h>

void setup() {
  Serial.begin(9600);
  initPins();
  initWifi();
  initWeb();
}

void loop() {}