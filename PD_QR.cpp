#include "PD_QR.h"

#include "qrcode.h"

void pdDrawQR(String text) {
  QRCode qrcode;
  const int qrVersion = 3;
  uint8_t qrcodeData[qrcode_getBufferSize(qrVersion)];

  qrcode_initText(&qrcode, qrcodeData, qrVersion, ECC_LOW, text.c_str());

  int scale = 4;
  int qrSize = qrcode.size * scale;
  int startX = (SAFE_W - qrSize) / 2;
  int startY = 34;

  tft.fillRect(startX - 4, startY - 4, qrSize + 8, qrSize + 8, TFT_WHITE);

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        tft.fillRect(startX + x * scale, startY + y * scale, scale, scale, TFT_BLACK);
      }
    }
  }
}

