#pragma once

#include "main.h"

class AutonomousSelector {
private:
    static AutonomousSelector* instance;
    int currentRoutine;
    const char* routineNames[10] = {
        "Four Ring Ring Rush",
        "Two Ring Safe Ring",
        "Three Ring Mogo Rush",
        "Regional Solo AWP Mogo Side",
        "Two Ring Safe Mogo",
        "Skills",
        "Disabled"
    };
    const char* routineNotes[10] = {
        "Notes: Align 7 regular nibs from wall, taped line to inner edge closer to rings",
        "Notes: Align with right wall",
        "Notes: Align with center line",
        "Notes: Align with mogo clamp center and inner tile edge",
        "Notes: Align with mogo clamp center and inner tile edge",
        "Notes: Align 9 by 9 nibs from corner",
        "Notes: No autonomous"
    };
    int routineCount = sizeof(routineNames) / sizeof(routineNames[0]);

    AutonomousSelector(); // Private constructor for singleton

public:
    static AutonomousSelector* getInstance();
    void nextRoutine();
    void previousRoutine(); 
    void updateDisplay();
    void runSelectedAutonomous();
    void toggleAllianceColor(); // New function to toggle alliance color
};

// Function to initialize the selector
void initializeSelector();

extern AutonomousSelector* selector;