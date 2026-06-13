#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "models.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA     21
#define OLED_SCL     22
#define OLED_ADDRESS 0x3C

extern Adafruit_SSD1306 display;

bool displayInit();
void displayText(const char* text, int x = 10, int y = 20, uint8_t size = 1);
void displayShowLines(const String& line1, const String& line2 = "", const String& line3 = "", const String& line4 = "");
void displayClear();

#endif
