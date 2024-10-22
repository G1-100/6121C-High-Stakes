#pragma once
#include "main.h"

extern double RESTANGLE;
extern double STOP1;
extern double STOP2;

extern bool LBLoopActive;

void LBExtend(int point);

void LBRetract();

void LBLoop();