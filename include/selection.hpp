/*#pragma once

#include "main.h"

// Autonomous routine options
enum class AutonomousRoutine {
    RING_BLUE,
    RING_RED,
    MOGO_BLUE,
    MOGO_RED, 
    SOLO_AWP_BLUE,
    SOLO_AWP_RED,
    RUSH_MOGO_BLUE,
    RUSH_MOGO_RED,
    SKILLS,
    DISABLED
};

class AutonomousSelector {
private:
    static AutonomousSelector* instance;
    AutonomousRoutine currentRoutine;
    const char* routineNames[10];

    AutonomousSelector(); // Private constructor for singleton

public:
    static AutonomousSelector* getInstance();
    void nextRoutine();
    void previousRoutine(); 
    void updateDisplay();
    void runSelectedAutonomous();
};

// Function to initialize the selector
void initializeSelector();

AutonomousSelector* selector;*/