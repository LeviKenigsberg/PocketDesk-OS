#pragma once
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Preferences.h>

extern TFT_eSPI tft;
extern Preferences prefs;

#define TOUCH_IRQ 36
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CLK 25
#define TOUCH_CS 33

#define BG TFT_BLACK
#define FG TFT_WHITE
#define ACCENT TFT_CYAN
#define BTN TFT_DARKGREY

#define SAFE_W 240
#define SAFE_H 205

