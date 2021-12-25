#include <Arduino.h>
#include <analogWrite.h>
#include <settings.h>

const int BLUE = LED_PIN_RED;
const int RED  = LED_PIN_GREEN;
const int GREEN = LED_PIN_BLUE;
int lastColorR = 255;
int lastColorG = 255;
int lastColorB = 255;

void initPins() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
}

void fadeTo(int fromR, int fromG, int fromB, int toR, int toB, int toG, int interval, int steps) {
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
      analogWrite(RED, toWriteR);
      analogWrite(GREEN, toWriteG);
      analogWrite(BLUE, toWriteB);
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

void writeToStrip(int r,int g,int b, String state) {
  if (state == "LED on") {
    fadeTo(lastColorR, lastColorG, lastColorB, r, g, b, 10, 50);
  }
  lastColorR = r;
  lastColorG = g;
  lastColorB = b;
}