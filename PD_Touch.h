#pragma once
#include "PD_Config.h"

void pdInitDisplay();
void pdInitTouch();
void pdRunCalibration();
void pdLoadCalibration();
bool pdGetTouch(int &x, int &y);

