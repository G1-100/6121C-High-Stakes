#pragma once
#include "main.h"

extern double RESTANGLE;
extern double STOP1;
extern double STOP1_5;
extern double STOP2;
extern double STOP3;

extern double LBState;
extern double REST;
extern double PROPPED;
extern double EXTENDED;
extern double SEMIEXTENDED;
extern double FULLEXTENDED;

extern bool LBLoopActive;

extern bool calledLBReset;

extern double LBAutonGoal;

extern bool intakeUnstuckActivated;

void LBExtend(double point);

void ChangeLBState(double goal);

void LBReset();

void LBRetract();

void LBLoop();

void callLBReset();