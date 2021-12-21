#include <Arduino.h>
#include <analogWrite.h>

const int BLUE = 2;
const int RED  = 4;
const int GREEN = 5;

int lastColorR = 255;
int lastColorG = 255;
int lastColorB = 255;

void initPins() {
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}

void turnOn() {
  analogWrite(RED, lastColorR);
  analogWrite(GREEN, lastColorG);
  analogWrite(BLUE, lastColorB);
}

void turnOff() {
  analogWrite(RED, 0);
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
}

void writeToStrip(int r,int g,int b) {
  lastColorR = r;
  lastColorG = g;
  lastColorB = b;
  analogWrite(RED, r);
  analogWrite(GREEN, g);
  analogWrite(BLUE, b);
}