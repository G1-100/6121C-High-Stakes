#pragma once
#include "main.h"

extern bool ColorLoopActive;

extern bool wrongColorDetected;

extern double ambientHue;

void initColorSort();

void doColorSort();

void intakeUntilColor();

void colorSortLoop();

 void activateColorSort();