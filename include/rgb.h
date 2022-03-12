#include <Arduino.h>

void initPins();
void turnOn();
void turnOff();
void writeToStrip(int r,int g,int b, bool isOn);
int getCurrColor(char color);