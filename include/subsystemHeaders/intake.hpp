#pragma once
#include "main.h"

// Header file for intake control system
// Manages the intake motor for collecting game elements
// Uses velocity control for consistent intake speed

// Constants
const int INTAKE_VELOCITY = 600; // Maximum motor velocity in RPM

// Helper functions
void setIntakeVelocity(int velocity);  // Sets intake motor velocity in RPM

// Driver Control Functions
void setIntakeMotors();  // Handles R1/R2 button input for intake control

