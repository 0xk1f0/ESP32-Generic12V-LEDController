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

void turnOn() {
  int increments = 0;
  int toWriteR;
  int toWriteG;
  int toWriteB;
  const long intrv = 10;
  unsigned long prevMillis = 0;
  while (increments < 100) {
    unsigned long currMillis = millis();
    toWriteR = map(increments, 0, 50, 0, lastColorR);
    toWriteG = map(increments, 0, 50, 0, lastColorG);
    toWriteB = map(increments, 0, 50, 0, lastColorB);
    if (currMillis - prevMillis >= intrv) {
      prevMillis = currMillis;
      analogWrite(RED, toWriteR);
      analogWrite(GREEN, toWriteG);
      analogWrite(BLUE, toWriteB);
      increments += 1;
    }
  }
}

void turnOff() {
  int increments = 50;
  int toWriteR;
  int toWriteG;
  int toWriteB;
  const long intrv = 10;
  unsigned long prevMillis = 0;
  while (increments > -1) {
    toWriteR = map(increments, 0, 50, 0, lastColorR);
    toWriteG = map(increments, 0, 50, 0, lastColorG);
    toWriteB = map(increments, 0, 50, 0, lastColorB);
    unsigned long currMillis = millis();
    if (currMillis - prevMillis >= intrv) {
      prevMillis = currMillis;
      analogWrite(RED, toWriteR);
      analogWrite(GREEN, toWriteG);
      analogWrite(BLUE, toWriteB);
      increments -= 1;
    }
  }
}

void writeToStrip(int r,int g,int b, String state) {
  lastColorR = r;
  lastColorG = g;
  lastColorB = b;
  if (state == "LED on") {
    analogWrite(RED, r);
    analogWrite(GREEN, g);
    analogWrite(BLUE, b);
  }
}