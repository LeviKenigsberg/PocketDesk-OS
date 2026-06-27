#pragma once
#include "PD_Config.h"

void pdWiFiBegin();
void pdWiFiLoop();
bool pdWiFiOK();
String pdWiFiStatus();
String pdWiFiIP();
void pdStartSetupPortal();
void pdForgetWiFi();
void pdReconnectWiFi();

