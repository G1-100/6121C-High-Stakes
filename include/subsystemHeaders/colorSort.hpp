#pragma once
#include "main.h"

extern bool ColorLoopActive;

extern bool wrongColorDetected;

extern double ambientHue;

extern double ambientRed;

extern double ambientBlue;

void initColorSort();

void doColorSort();

void intakeUntilColor();

void colorSortLoop();

void activateColorSort();

void stopColorUntilFunction();