#pragma once
#include "main.h"

extern double RESTANGLE;
extern double STOP1;
extern double STOP2;
extern double STOP3;

extern int LBState;
extern double REST;
extern double PROPPED;
extern double EXTENDED;
extern double FULLEXTENDED;

extern bool LBLoopActive;

extern bool calledLBReset;

extern int LBAutonGoal;

extern bool intakeUnstuckActivated;

void LBExtend(int point);

void ChangeLBState(int goal);

void LBReset();

void LBRetract();

void LBLoop();

void callLBReset();