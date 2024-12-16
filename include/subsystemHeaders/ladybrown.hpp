#pragma once
#include "main.h"

extern double RESTANGLE;
extern double STOP1;
extern double STOP2;

extern int LBState;
extern double REST;
extern double PROPPED;
extern double EXTENDED;

extern bool LBLoopActive;

extern int LBAutonGoal;

void LBExtend(int point);

void ChangeLBState(int goal);

void LBReset();

void LBRetract();

void LBLoop();