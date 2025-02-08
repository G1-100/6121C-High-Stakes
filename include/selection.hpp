#pragma once

#include "main.h"

class AutonomousSelector {
private:
    static AutonomousSelector* instance;
    const char* routineNames[10] = {
        "Four Ring Ring Rush",
        "Two Ring Safe Ring",
        "Three Ring Mogo Rush",
        "Regional Solo AWP Mogo Side",
        "Four Ring Safe Mogo Side",
        "Two Ring Safe Mogo",
        "Skills",
        "Disabled"
    };
    const char* routineNotes[10] = {
        "Notes: Align 7 regular nibs from wall, taped line to inner edge closer to rings",
        "Notes: Align with right wall",
        "Notes: Align with center line",
        "Notes: Align with 13 nibs from corner, towards AWS",
        "Notes: Align with 13 nibs from corner, towards AWS",
        "Notes: Align with mogo clamp center and inner tile edge",
        "Notes: Align 9 by 9 nibs from corner",
        "Notes: No autonomous"
    };
    int routineCount = 8;

    AutonomousSelector(); // Private constructor for singleton

public:
    static AutonomousSelector* getInstance();
    void nextRoutine();
    int currentRoutine;
    void previousRoutine(); 
    void updateDisplay();
    void runSelectedAutonomous();
    void toggleAllianceColor(); // New function to toggle alliance color
};

// Function to initialize the selector
void initializeSelector();

extern AutonomousSelector* selector;