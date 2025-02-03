#pragma once
#include "main.h"

extern bool ColorLoopActive;

extern bool wrongColorDetected;

extern bool colorLoopStarted;

extern double ambientHue;

extern double ambientProximity;

extern bool colorUntilActivated;
extern int ringsSeen;
extern int colorUntilRings;

extern bool safeScoring;
extern double prevHeading;

void initColorSort();

void doColorSort();

void colorSortLoop();

void stopColorUntilFunction();

void startColorUntil(int rings);
