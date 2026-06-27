#pragma once
#include "PD_Config.h"
#include "PD_Touch.h"

enum Screen {
  HOME,
  CLOCK_APP,
  MESSAGE_APP,
  QR_APP,
  STOPWATCH_APP,
  TIMER_APP,
  DICE_APP,
  COUNTER_APP,
  LIGHT_APP,
  SETTINGS_APP,
  ABOUT_APP,
  TEXT_INPUT_APP
};

extern Screen currentScreen;
extern String webMessage;
extern String qrText;

void pdDrawScreen();
void pdHandleTouch();

