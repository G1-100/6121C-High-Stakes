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
        "Align with left wall",
        "Align with right wall",
        "Align with center line",
        "Align with mogo",
        "Align with ring",
        "No alignment needed",
        "No autonomous"
    };
    int routineCount;

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