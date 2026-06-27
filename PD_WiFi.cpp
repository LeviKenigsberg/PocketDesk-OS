#include <FS.h>
using namespace fs;

#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "PD_WiFi.h"
#include "PD_UI.h"

WebServer server(80);

bool wifiConnected = false;
bool setupPortal = false;

String htmlPage(String body) {
  String page = "";
  page += "<!DOCTYPE html><html><head>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>PocketDesk OS</title>";
  page += "<style>";
  page += "body{font-family:Arial;background:#111;color:white;padding:20px;}";
  page += "textarea,input,button{width:100%;font-size:18px;padding:10px;margin:8px 0;box-sizing:border-box;}";
  page += "button{background:#00bcd4;border:0;border-radius:8px;color:#000;font-weight:bold;}";
  page += "</style></head><body>";
  page += body;
  page += "</body></html>";
  return page;
}

String setupPage() {
  String body = "";
  body += "<h1>PocketDesk WiFi Setup</h1>";
  body += "<p>Enter your home WiFi. It will be saved on the device.</p>";
  body += "<form action='/savewifi' method='POST'>";
  body += "<input name='ssid' placeholder='WiFi name'>";
  body += "<input name='pass' placeholder='WiFi password' type='password'>";
  body += "<button>Save WiFi</button>";
  body += "</form>";
  return htmlPage(body);
}

String controlPage() {
  String body = "";
  body += "<h1>PocketDesk OS</h1>";
  body += "<p>IP: " + WiFi.localIP().toString() + "</p>";

  body += "<form action='/message' method='POST'>";
  body += "<h2>Message</h2>";
  body += "<textarea name='msg'>" + webMessage + "</textarea>";
  body += "<button>Show Message</button></form>";

  body += "<form action='/qr' method='POST'>";
  body += "<h2>QR Code</h2>";
  body += "<textarea name='qr'>" + qrText + "</textarea>";
  body += "<button>Show QR Code</button></form>";

  body += "<form action='/clock' method='POST'><button>Show Clock</button></form>";
  body += "<form action='/home' method='POST'><button>Show Home</button></form>";

  return htmlPage(body);
}

void setupServerRoutes() {
  server.on("/", []() {
    if (setupPortal) server.send(200, "text/html", setupPage());
    else server.send(200, "text/html", controlPage());
  });

  server.on("/savewifi", HTTP_POST, []() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    prefs.putString("wifi_ssid", ssid);
    prefs.putString("wifi_pass", pass);

    String body = "<h1>Saved!</h1><p>Restart the CYD. It should connect to your WiFi.</p>";
    server.send(200, "text/html", htmlPage(body));
  });

  server.on("/message", HTTP_POST, []() {
    webMessage = server.arg("msg");
    currentScreen = MESSAGE_APP;
    pdDrawScreen();
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/qr", HTTP_POST, []() {
    qrText = server.arg("qr");
    currentScreen = QR_APP;
    pdDrawScreen();
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/clock", HTTP_POST, []() {
    currentScreen = CLOCK_APP;
    pdDrawScreen();
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/home", HTTP_POST, []() {
    currentScreen = HOME;
    pdDrawScreen();
    server.sendHeader("Location", "/");
    server.send(303);
  });
}

void pdStartSetupPortal() {
  server.stop();

  WiFi.mode(WIFI_AP);
  WiFi.softAP("PocketDesk-Setup");

  setupPortal = true;
  wifiConnected = false;

  setupServerRoutes();
  server.begin();

  tft.fillScreen(BG);
  tft.setTextColor(ACCENT, BG);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("WiFi Setup", SAFE_W / 2, 8, 2);

  tft.setTextColor(FG, BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Connect to:", SAFE_W / 2, 55, 2);
  tft.drawString("PocketDesk-Setup", SAFE_W / 2, 85, 2);
  tft.drawString("Then open:", SAFE_W / 2, 125, 2);
  tft.drawString("192.168.4.1", SAFE_W / 2, 155, 2);
}

void pdReconnectWiFi() {
  server.stop();

  String ssid = prefs.getString("wifi_ssid", "");
  String pass = prefs.getString("wifi_pass", "");

  wifiConnected = false;
  setupPortal = false;

  if (ssid == "") {
    pdStartSetupPortal();
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
  }

  wifiConnected = WiFi.status() == WL_CONNECTED;

  if (wifiConnected) {
    configTime(-7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    setupServerRoutes();
    server.begin();
  } else {
    pdStartSetupPortal();
  }
}

void pdWiFiBegin() {
  pdReconnectWiFi();
}

void pdWiFiLoop() {
  server.handleClient();
}

bool pdWiFiOK() {
  return wifiConnected;
}

String pdWiFiIP() {
  if (wifiConnected) return WiFi.localIP().toString();
  if (setupPortal) return "192.168.4.1";
  return "Offline";
}

String pdWiFiStatus() {
  if (wifiConnected) return WiFi.localIP().toString();
  if (setupPortal) return "Setup: 192.168.4.1";
  return "WiFi Offline";
}

void pdForgetWiFi() {
  prefs.remove("wifi_ssid");
  prefs.remove("wifi_pass");
  pdStartSetupPortal();
}

