#include "PD_Config.h"
#include "PD_Touch.h"
#include "PD_UI.h"
#include "PD_WiFi.h"

void setup() {
  Serial.begin(115200);

  pdInitDisplay();
  pdInitTouch();

  prefs.begin("pocketdesk", false);

  if (!prefs.getBool("calDone", false)) {
    pdRunCalibration();
  }

  pdLoadCalibration();
  randomSeed(analogRead(34));

  pdWiFiBegin();

  currentScreen = HOME;
  pdDrawScreen();
}

void loop() {
  pdWiFiLoop();
  pdHandleTouch();

  static unsigned long lastDraw = 0;
  if (millis() - lastDraw > 1000) {
    lastDraw = millis();

    if (currentScreen == CLOCK_APP || currentScreen == STOPWATCH_APP || currentScreen == TIMER_APP) {
      pdDrawScreen();
    }
  }
}

