#include <Arduino.h>
#include <analogWrite.h>
#include <settings/pinSettings.h>
#include <rgb.h>

int lastColorR = 255;
int lastColorG = 255;
int lastColorB = 255;

void initPins() {
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  pinMode(LED_PIN_BLUE, OUTPUT);
  analogWrite(LED_PIN_RED, 0);
  analogWrite(LED_PIN_GREEN, 0);
  analogWrite(LED_PIN_BLUE, 0);
}

void fadeTo(int fromR, int fromG, int fromB, int toR, int toG, int toB, int interval, int steps) {
  int increments = 0;
  int toWriteR;
  int toWriteG;
  int toWriteB;
  const long intrv = interval;
  unsigned long prevMillis = 0;
  while (increments <= steps) {
    unsigned long currMillis = millis();
    toWriteR = map(increments, 0, steps, fromR, toR);
    toWriteG = map(increments, 0, steps, fromG, toG);
    toWriteB = map(increments, 0, steps, fromB, toB);
    if (currMillis - prevMillis >= intrv) {
      prevMillis = currMillis;
      analogWrite(LED_PIN_RED, toWriteR);
      analogWrite(LED_PIN_GREEN, toWriteG);
      analogWrite(LED_PIN_BLUE, toWriteB);
      increments += 1;
    }
  }
}

void turnOn() {
  fadeTo(0, 0, 0, lastColorR, lastColorG, lastColorB, 10, 50);
}

void turnOff() {
  fadeTo(lastColorR, lastColorG, lastColorB, -1, -1, -1, 10, 50);
}

void writeToStrip(int r,int g,int b, bool isOn) {
  if (isOn == true) {
    fadeTo(lastColorR, lastColorG, lastColorB, r, g, b, 10, 50);
  }
  lastColorR = r;
  lastColorG = g;
  lastColorB = b;
}

int getCurrColor(char color) {
    switch (color) {
        case 'R':
            return lastColorR;
            break;
        case 'G':
            return lastColorG;
            break;
        case 'B':
            return lastColorB;
            break;
    }
}