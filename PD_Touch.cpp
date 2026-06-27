#include "PD_Touch.h"

TFT_eSPI tft = TFT_eSPI();
Preferences prefs;

SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

int rawXMin, rawXMax, rawYMin, rawYMax;

TS_Point waitRawTouch() {
  while (!ts.touched()) delay(10);
  TS_Point p = ts.getPoint();
  delay(400);
  return p;
}

void pdInitDisplay() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(BG);
}

void pdInitTouch() {
  touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(touchSPI);
  ts.setRotation(1);
}

void drawCalTarget(int x, int y, String label) {
  tft.fillScreen(BG);
  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Touch Calibration", SAFE_W / 2, 25, 2);
  tft.drawString(label, SAFE_W / 2, 55, 2);
  tft.drawLine(x - 12, y, x + 12, y, ACCENT);
  tft.drawLine(x, y - 12, x, y + 12, ACCENT);
  tft.drawCircle(x, y, 16, ACCENT);
}

void pdRunCalibration() {
  TS_Point tl, tr, bl, br;

  drawCalTarget(25, 25, "Tap top left");
  tl = waitRawTouch();

  drawCalTarget(SAFE_W - 25, 25, "Tap top right");
  tr = waitRawTouch();

  drawCalTarget(25, SAFE_H - 25, "Tap bottom left");
  bl = waitRawTouch();

  drawCalTarget(SAFE_W - 25, SAFE_H - 25, "Tap bottom right");
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

void pdLoadCalibration() {
  rawXMin = prefs.getInt("rxmin", 200);
  rawXMax = prefs.getInt("rxmax", 3800);
  rawYMin = prefs.getInt("rymin", 200);
  rawYMax = prefs.getInt("rymax", 3800);
}

bool pdGetTouch(int &x, int &y) {
  if (!ts.touched()) return false;

  TS_Point p = ts.getPoint();

  x = map(p.y, rawXMin, rawXMax, 0, SAFE_W);
  y = map(p.x, rawYMin, rawYMax, 0, SAFE_H);

  x = constrain(x, 0, SAFE_W - 1);
  y = constrain(y, 0, SAFE_H - 1);

  delay(170);
  return true;
}

