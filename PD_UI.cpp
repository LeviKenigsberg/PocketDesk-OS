#include "PD_UI.h"
#include "PD_WiFi.h"
#include "PD_QR.h"
#include <time.h>

Screen currentScreen = HOME;

String webMessage = "Hello from PocketDesk OS!";
String qrText = "PocketDesk OS";

String inputText = "";
String inputTarget = "";
Screen inputReturn = HOME;

unsigned long swStart = 0;
unsigned long swSaved = 0;
bool swRunning = false;

int timerSeconds = 300;
unsigned long timerStarted = 0;
bool timerRunning = false;

int counterValue = 0;
int diceValue = 1;

void drawButton(int x, int y, int w, int h, String label) {
  tft.fillRoundRect(x, y, w, h, 7, BTN);
  tft.drawRoundRect(x, y, w, h, 7, ACCENT);
  tft.setTextColor(FG, BTN);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(label, x + w / 2, y + h / 2, 2);
}

bool inBox(int x, int y, int bx, int by, int bw, int bh) {
  return x >= bx && x <= bx + bw && y >= by && y <= by + bh;
}

void title(String text) {
  tft.fillScreen(BG);
  tft.setTextColor(ACCENT, BG);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(text, SAFE_W / 2, 6, 2);
}

void backButton() {
  drawButton(5, 172, 70, 28, "Back");
}

void drawHome() {
  title("PocketDesk");

  drawButton(15, 30, 95, 28, "Clock");
  drawButton(130, 30, 95, 28, "Message");

  drawButton(15, 65, 95, 28, "QR");
  drawButton(130, 65, 95, 28, "Stopwatch");

  drawButton(15, 100, 95, 28, "Timer");
  drawButton(130, 100, 95, 28, "Dice");

  drawButton(15, 135, 95, 28, "Counter");
  drawButton(130, 135, 95, 28, "Settings");

  tft.setTextColor(pdWiFiOK() ? TFT_GREEN : TFT_ORANGE, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(pdWiFiStatus(), SAFE_W / 2, 195, 2);
}

void drawClock() {
  title("WiFi Clock");
  backButton();

  if (!pdWiFiOK()) {
    tft.setTextColor(TFT_RED, BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("WiFi not connected", SAFE_W / 2, 90, 2);
    return;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    tft.setTextColor(TFT_ORANGE, BG);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Getting time...", SAFE_W / 2, 90, 2);
    return;
  }

  int hour = timeinfo.tm_hour;
  String ampm = "AM";
  if (hour >= 12) ampm = "PM";
  hour = hour % 12;
  if (hour == 0) hour = 12;

  char timeStr[24];
  char dateStr[24];

  sprintf(timeStr, "%02d:%02d:%02d %s", hour, timeinfo.tm_min, timeinfo.tm_sec, ampm.c_str());
  sprintf(dateStr, "%02d/%02d/%04d", timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_year + 1900);

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(timeStr, SAFE_W / 2, 80, 4);

  tft.setTextColor(ACCENT, BG);
  tft.drawString(dateStr, SAFE_W / 2, 125, 2);
}

void drawMessage() {
  title("Message");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawRoundRect(10, 48, 220, 88, 8, ACCENT);
  tft.drawString(webMessage, SAFE_W / 2, 92, 2);

  drawButton(30, 145, 80, 28, "Edit");
  drawButton(130, 145, 80, 28, "Clear");
}

void drawQR() {
  title("QR Code");
  backButton();
  pdDrawQR(qrText);

  drawButton(30, 160, 80, 28, "Edit");
  drawButton(130, 160, 80, 28, "Reset");
}

void drawStopwatch() {
  title("Stopwatch");
  backButton();

  unsigned long total = swSaved;
  if (swRunning) total += millis() - swStart;

  int s = total / 1000;
  char buf[20];
  sprintf(buf, "%02d:%02d:%02d", s / 3600, (s / 60) % 60, s % 60);

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(buf, SAFE_W / 2, 80, 4);

  drawButton(25, 125, 80, 32, swRunning ? "Pause" : "Start");
  drawButton(135, 125, 80, 32, "Reset");
}

void drawTimer() {
  title("Timer");
  backButton();

  int remaining = timerSeconds;
  if (timerRunning) {
    int elapsed = (millis() - timerStarted) / 1000;
    remaining = max(0, timerSeconds - elapsed);
    if (remaining == 0) timerRunning = false;
  }

  char buf[20];
  sprintf(buf, "%02d:%02d", remaining / 60, remaining % 60);

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(buf, SAFE_W / 2, 70, 4);

  drawButton(15, 110, 60, 30, "+1m");
  drawButton(90, 110, 60, 30, "+5m");
  drawButton(165, 110, 60, 30, "Reset");
  drawButton(80, 145, 80, 30, timerRunning ? "Pause" : "Start");
}

void drawDice() {
  title("Dice");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(String(diceValue), SAFE_W / 2, 80, 7);

  drawButton(65, 135, 110, 35, "Roll");
}

void drawCounter() {
  title("Counter");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(String(counterValue), SAFE_W / 2, 75, 7);

  drawButton(20, 130, 60, 32, "-");
  drawButton(90, 130, 60, 32, "Reset");
  drawButton(160, 130, 60, 32, "+");
}

void drawLight() {
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Tap to go back", SAFE_W / 2, 100, 2);
}

void drawSettings() {
  title("Settings");
  backButton();

  drawButton(25, 38, 190, 28, "WiFi Setup Portal");
  drawButton(25, 72, 190, 28, "Forget WiFi");
  drawButton(25, 106, 190, 28, "Recalibrate Touch");
  drawButton(25, 140, 190, 28, "Screen Light");
}

void drawAbout() {
  title("About");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("PocketDesk OS v0.2.0", 15, 45, 2);
  tft.drawString("WiFi Tools Update", 15, 70, 2);
  tft.drawString("No hardcoded WiFi", 15, 95, 2);
  tft.drawString("Clock, Web Message,", 15, 125, 2);
  tft.drawString("QR, Timer, Dice", 15, 150, 2);
}

void drawKeyboard() {
  title(inputTarget == "qr" ? "Edit QR" : "Edit Message");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(TL_DATUM);
  tft.drawString(inputText, 8, 30, 2);

  String keys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./:-_";
  int i = 0;

  for (int yy = 58; yy < 150; yy += 30) {
    for (int xx = 8; xx < 232; xx += 28) {
      if (i < keys.length()) {
        drawButton(xx, yy, 24, 24, String(keys[i]));
        i++;
      }
    }
  }

  drawButton(8, 170, 55, 28, "Space");
  drawButton(68, 170, 50, 28, "Del");
  drawButton(123, 170, 50, 28, "Save");
  drawButton(178, 170, 50, 28, "Clear");
}

void pdDrawScreen() {
  if (currentScreen == HOME) drawHome();
  else if (currentScreen == CLOCK_APP) drawClock();
  else if (currentScreen == MESSAGE_APP) drawMessage();
  else if (currentScreen == QR_APP) drawQR();
  else if (currentScreen == STOPWATCH_APP) drawStopwatch();
  else if (currentScreen == TIMER_APP) drawTimer();
  else if (currentScreen == DICE_APP) drawDice();
  else if (currentScreen == COUNTER_APP) drawCounter();
  else if (currentScreen == LIGHT_APP) drawLight();
  else if (currentScreen == SETTINGS_APP) drawSettings();
  else if (currentScreen == ABOUT_APP) drawAbout();
  else if (currentScreen == TEXT_INPUT_APP) drawKeyboard();
}

void openKeyboard(String target, String existing, Screen returnScreen) {
  inputTarget = target;
  inputText = existing;
  inputReturn = returnScreen;
  currentScreen = TEXT_INPUT_APP;
  pdDrawScreen();
}

void handleKeyboard(int x, int y) {
  if (inBox(x, y, 5, 172, 70, 28)) {
    currentScreen = inputReturn;
    pdDrawScreen();
    return;
  }

  if (inBox(x, y, 8, 170, 55, 28)) inputText += " ";
  else if (inBox(x, y, 68, 170, 50, 28)) {
    if (inputText.length() > 0) inputText.remove(inputText.length() - 1);
  }
  else if (inBox(x, y, 123, 170, 50, 28)) {
    if (inputTarget == "message") webMessage = inputText;
    if (inputTarget == "qr") qrText = inputText;
    currentScreen = inputReturn;
    pdDrawScreen();
    return;
  }
  else if (inBox(x, y, 178, 170, 50, 28)) inputText = "";
  else {
    String keys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./:-_";
    int i = 0;

    for (int yy = 58; yy < 150; yy += 30) {
      for (int xx = 8; xx < 232; xx += 28) {
        if (i < keys.length() && inBox(x, y, xx, yy, 24, 24)) {
          inputText += keys[i];
        }
        i++;
      }
    }
  }

  pdDrawScreen();
}

void pdHandleTouch() {
  int x, y;
  if (!pdGetTouch(x, y)) return;

  if (currentScreen == TEXT_INPUT_APP) {
    handleKeyboard(x, y);
    return;
  }

  if (currentScreen == LIGHT_APP) {
    currentScreen = HOME;
    pdDrawScreen();
    return;
  }

  if (currentScreen != HOME && inBox(x, y, 5, 172, 70, 28)) {
    currentScreen = HOME;
    pdDrawScreen();
    return;
  }

  if (currentScreen == HOME) {
    if (inBox(x, y, 15, 30, 95, 28)) currentScreen = CLOCK_APP;
    else if (inBox(x, y, 130, 30, 95, 28)) currentScreen = MESSAGE_APP;
    else if (inBox(x, y, 15, 65, 95, 28)) currentScreen = QR_APP;
    else if (inBox(x, y, 130, 65, 95, 28)) currentScreen = STOPWATCH_APP;
    else if (inBox(x, y, 15, 100, 95, 28)) currentScreen = TIMER_APP;
    else if (inBox(x, y, 130, 100, 95, 28)) currentScreen = DICE_APP;
    else if (inBox(x, y, 15, 135, 95, 28)) currentScreen = COUNTER_APP;
    else if (inBox(x, y, 130, 135, 95, 28)) currentScreen = SETTINGS_APP;
    pdDrawScreen();
  }

  else if (currentScreen == MESSAGE_APP) {
    if (inBox(x, y, 30, 145, 80, 28)) openKeyboard("message", webMessage, MESSAGE_APP);
    if (inBox(x, y, 130, 145, 80, 28)) {
      webMessage = "";
      pdDrawScreen();
    }
  }

  else if (currentScreen == QR_APP) {
    if (inBox(x, y, 30, 160, 80, 28)) openKeyboard("qr", qrText, QR_APP);
    if (inBox(x, y, 130, 160, 80, 28)) {
      qrText = "PocketDesk OS";
      pdDrawScreen();
    }
  }

  else if (currentScreen == STOPWATCH_APP) {
    if (inBox(x, y, 25, 125, 80, 32)) {
      if (!swRunning) {
        swStart = millis();
        swRunning = true;
      } else {
        swSaved += millis() - swStart;
        swRunning = false;
      }
    }

    if (inBox(x, y, 135, 125, 80, 32)) {
      swSaved = 0;
      swRunning = false;
    }

    pdDrawScreen();
  }

  else if (currentScreen == TIMER_APP) {
    if (inBox(x, y, 15, 110, 60, 30)) timerSeconds += 60;
    if (inBox(x, y, 90, 110, 60, 30)) timerSeconds += 300;

    if (inBox(x, y, 165, 110, 60, 30)) {
      timerSeconds = 300;
      timerRunning = false;
    }

    if (inBox(x, y, 80, 145, 80, 30)) {
      if (!timerRunning) {
        timerStarted = millis();
        timerRunning = true;
      } else {
        int elapsed = (millis() - timerStarted) / 1000;
        timerSeconds = max(0, timerSeconds - elapsed);
        timerRunning = false;
      }
    }

    pdDrawScreen();
  }

  else if (currentScreen == DICE_APP) {
    diceValue = random(1, 7);
    pdDrawScreen();
  }

  else if (currentScreen == COUNTER_APP) {
    if (inBox(x, y, 20, 130, 60, 32)) counterValue--;
    if (inBox(x, y, 90, 130, 60, 32)) counterValue = 0;
    if (inBox(x, y, 160, 130, 60, 32)) counterValue++;
    pdDrawScreen();
  }

  else if (currentScreen == SETTINGS_APP) {
    if (inBox(x, y, 25, 38, 190, 28)) {
      pdStartSetupPortal();
      return;
    }

    if (inBox(x, y, 25, 72, 190, 28)) {
      pdForgetWiFi();
      return;
    }

    if (inBox(x, y, 25, 106, 190, 28)) {
      prefs.putBool("calDone", false);
      pdRunCalibration();
      pdLoadCalibration();
      currentScreen = HOME;
      pdDrawScreen();
    }

    if (inBox(x, y, 25, 140, 190, 28)) {
      currentScreen = LIGHT_APP;
      pdDrawScreen();
    }
  }
}

