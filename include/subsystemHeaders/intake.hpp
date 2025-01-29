#pragma once
#include "main.h"

// Header file for intake control system
// Manages the intake motor for collecting game elements
// Uses velocity control for consistent intake speed

// Constants
const int INTAKE_POWER = 12000; // Maximum motor power

extern bool stopDriverIntake;

// Helper functions
void setIntake(int power);  // Sets intake motor power

// Driver Control Functions
void setIntakeMotors();  // Handles R1/R2 button input for intake control

void setIntakeLift();  // Handles L1 button input for intake lift control