#pragma once

#include "main.h"

// Autonomous routine options
enum class AutonomousRoutine {
    Four_Ring_Ring_Rush,
    Two_Ring_Safe_Ring,
    Three_Ring_Mogo_Rush,
    Regional_Solo_AWP_Mogo_Side,
    Two_Ring_Safe_Mogo,
    Skills,
    Disabled
};

class AutonomousSelector {
private:
    static AutonomousSelector* instance;
    AutonomousRoutine currentRoutine;
    const char* routineNames[10];
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