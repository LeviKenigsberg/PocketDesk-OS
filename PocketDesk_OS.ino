#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Preferences.h>

TFT_eSPI tft = TFT_eSPI();
Preferences prefs;

#define TOUCH_IRQ 36
#define TOUCH_MOSI 32
#define TOUCH_MISO 39
#define TOUCH_CLK 25
#define TOUCH_CS 33

SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

#define BG TFT_BLACK
#define FG TFT_WHITE
#define ACCENT TFT_CYAN
#define BTN TFT_DARKGREY

#define SAFE_W 240
#define SAFE_H 205

int rawXMin, rawXMax, rawYMin, rawYMax;

enum Screen {
  HOME,
  STOPWATCH,
  TIMER,
  DICE,
  COUNTER,
  LIGHT,
  SETTINGS,
  ABOUT
};

Screen screen = HOME;

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

TS_Point waitRawTouch() {
  while (!ts.touched()) delay(10);
  TS_Point p = ts.getPoint();
  delay(400);
  return p;
}

void drawTarget(int x, int y, String label) {
  tft.fillScreen(BG);
  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Touch Calibration", SAFE_W / 2, 25, 2);
  tft.drawString(label, SAFE_W / 2, 55, 2);

  tft.drawLine(x - 12, y, x + 12, y, ACCENT);
  tft.drawLine(x, y - 12, x, y + 12, ACCENT);
  tft.drawCircle(x, y, 16, ACCENT);
}

void runCalibration() {
  TS_Point tl, tr, bl, br;

  drawTarget(25, 25, "Tap top left");
  tl = waitRawTouch();

  drawTarget(SAFE_W - 25, 25, "Tap top right");
  tr = waitRawTouch();

  drawTarget(25, SAFE_H - 25, "Tap bottom left");
  bl = waitRawTouch();

  drawTarget(SAFE_W - 25, SAFE_H - 25, "Tap bottom right");
  br = waitRawTouch();

  rawXMin = (tl.y + bl.y) / 2;
  rawXMax = (tr.y + br.y) / 2;
  rawYMin = (tl.x + tr.x) / 2;
  rawYMax = (bl.x + br.x) / 2;

  prefs.putInt("rxmin", rawXMin);
  prefs.putInt("rxmax", rawXMax);
  prefs.putInt("rymin", rawYMin);
  prefs.putInt("rymax", rawYMax);
  prefs.putBool("calDone", true);

  tft.fillScreen(BG);
  tft.setTextColor(TFT_GREEN, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Calibration Saved", SAFE_W / 2, 100, 2);
  delay(1000);
}

void loadCalibration() {
  rawXMin = prefs.getInt("rxmin", 200);
  rawXMax = prefs.getInt("rxmax", 3800);
  rawYMin = prefs.getInt("rymin", 200);
  rawYMax = prefs.getInt("rymax", 3800);
}

bool getTouch(int &x, int &y) {
  if (!ts.touched()) return false;

  TS_Point p = ts.getPoint();

  x = map(p.y, rawXMin, rawXMax, 0, SAFE_W);
  y = map(p.x, rawYMin, rawYMax, 0, SAFE_H);

  x = constrain(x, 0, SAFE_W - 1);
  y = constrain(y, 0, SAFE_H - 1);

  delay(180);
  return true;
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

  drawButton(15, 35, 95, 32, "Stopwatch");
  drawButton(130, 35, 95, 32, "Timer");

  drawButton(15, 78, 95, 32, "Dice");
  drawButton(130, 78, 95, 32, "Counter");

  drawButton(15, 121, 95, 32, "Light");
  drawButton(130, 121, 95, 32, "Settings");

  drawButton(72, 164, 95, 32, "About");
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

  drawButton(25, 60, 190, 35, "Recalibrate Touch");
  drawButton(25, 110, 190, 35, "Reset Counter");
}

void drawAbout() {
  title("About");
  backButton();

  tft.setTextColor(FG, BG);
  tft.setTextDatum(TL_DATUM);

  tft.drawString("PocketDesk OS v0.1", 20, 45, 2);
  tft.drawString("Offline desk toolkit", 20, 70, 2);
  tft.drawString("Real features only.", 20, 95, 2);
  tft.drawString("Touch calibration saves", 20, 125, 2);
  tft.drawString("after first setup.", 20, 150, 2);
}

void drawScreen() {
  if (screen == HOME) drawHome();
  else if (screen == STOPWATCH) drawStopwatch();
  else if (screen == TIMER) drawTimer();
  else if (screen == DICE) drawDice();
  else if (screen == COUNTER) drawCounter();
  else if (screen == LIGHT) drawLight();
  else if (screen == SETTINGS) drawSettings();
  else if (screen == ABOUT) drawAbout();
}

void handleTouch() {
  int x, y;
  if (!getTouch(x, y)) return;

  if (screen == LIGHT) {
    screen = HOME;
    drawScreen();
    return;
  }

  if (screen != HOME && inBox(x, y, 5, 172, 70, 28)) {
    screen = HOME;
    drawScreen();
    return;
  }

  if (screen == HOME) {
    if (inBox(x, y, 15, 35, 95, 32)) screen = STOPWATCH;
    else if (inBox(x, y, 130, 35, 95, 32)) screen = TIMER;
    else if (inBox(x, y, 15, 78, 95, 32)) screen = DICE;
    else if (inBox(x, y, 130, 78, 95, 32)) screen = COUNTER;
    else if (inBox(x, y, 15, 121, 95, 32)) screen = LIGHT;
    else if (inBox(x, y, 130, 121, 95, 32)) screen = SETTINGS;
    else if (inBox(x, y, 72, 164, 95, 32)) screen = ABOUT;

    drawScreen();
  }

  else if (screen == STOPWATCH) {
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

    drawScreen();
  }

  else if (screen == TIMER) {
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

    drawScreen();
  }

  else if (screen == DICE) {
    if (inBox(x, y, 65, 135, 110, 35)) {
      diceValue = random(1, 7);
      drawScreen();
    }
  }

  else if (screen == COUNTER) {
    if (inBox(x, y, 20, 130, 60, 32)) counterValue--;
    if (inBox(x, y, 90, 130, 60, 32)) counterValue = 0;
    if (inBox(x, y, 160, 130, 60, 32)) counterValue++;

    drawScreen();
  }

  else if (screen == SETTINGS) {
    if (inBox(x, y, 25, 60, 190, 35)) {
      prefs.putBool("calDone", false);
      runCalibration();
      loadCalibration();
      screen = HOME;
    }

    if (inBox(x, y, 25, 110, 190, 35)) {
      counterValue = 0;
    }

    drawScreen();
  }
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BG);

  touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);

  prefs.begin("pocketdesk", false);

  if (!prefs.getBool("calDone", false)) {
    runCalibration();
  }

  loadCalibration();

  randomSeed(analogRead(34));

  drawHome();
}

void loop() {
  handleTouch();

  static unsigned long lastDraw = 0;

  if (millis() - lastDraw > 1000) {
    lastDraw = millis();

    if (screen == STOPWATCH || screen == TIMER) {
      drawScreen();
    }
  }
}

