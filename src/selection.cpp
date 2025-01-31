#include "selection.hpp"
#include "pros/misc.h"
#include "pros/rtos.hpp"

// Initialize static member
AutonomousSelector* AutonomousSelector::instance = nullptr;

// Implementation of the singleton constructor
AutonomousSelector::AutonomousSelector() : currentRoutine(AutonomousRoutine::Disabled) {
    const char* names[] = {
    "Four Ring Ring Rush",
    "Two Ring Safe Ring",
    "Three Ring Mogo Rush",
    "Regional Solo AWP Mogo Side",
    "Two Ring Safe Mogo",
    "Skills",
    "Disabled"
    };
    int routineCount = 7;
    std::copy(std::begin(names), std::end(names), routineNames);
    
    // Initialize selector UI
    pros::lcd::initialize();
    updateDisplay();
}

// Implement the rest of the member functions
// ...rest of the implementation remains the same...

AutonomousSelector* AutonomousSelector::getInstance() {
    if (instance == nullptr) {
        instance = new AutonomousSelector();
    }
    return instance;
}

// Define the selector variable
AutonomousSelector* selector = nullptr;

void AutonomousSelector::nextRoutine() {
    int next = static_cast<int>(currentRoutine) + 1;
    if (next >= 7) next = 0;
    currentRoutine = static_cast<AutonomousRoutine>(next);
    updateDisplay();
}

void AutonomousSelector::previousRoutine() {
    int prev = static_cast<int>(currentRoutine) - 1;
    if (prev < 0) prev = routineCount - 1;
    currentRoutine = static_cast<AutonomousRoutine>(prev);
    updateDisplay();
}

void AutonomousSelector::toggleAllianceColor() {
    allianceColorBlue = !allianceColorBlue;
    updateDisplay();
}

void AutonomousSelector::updateDisplay() {
    std::cout <<"Currrent Routine: " << routineNames[static_cast<int>(currentRoutine)] << "\n";
    std::cout << "routine count: " << routineCount << "\n";
    // Clear each line individually since there's no clear() function
    pros::lcd::clear_line(1);
    pros::lcd::clear_line(2);
    pros::lcd::clear_line(3);
    
    pros::lcd::set_text(1, "Selected Autonomous:");
    pros::lcd::set_text(2, routineNames[static_cast<int>(currentRoutine)]);
    
    // Display competition status
    std::string status = "Status: ";
    if (pros::competition::is_connected()) {
        status += "Connected";
        if (pros::competition::is_autonomous()) {
            status += " (Autonomous)";
        } else if (pros::competition::is_disabled()) {
            status += " (Disabled)";
        } else {
            status += " (Opcontrol)";
        }
    } else {
        status += "Not Connected";
    }
    pros::lcd::set_text(3, status);

    // Display alliance color
    pros::lcd::set_text(4, allianceColorBlue ? "Alliance: Blue" : "Alliance: Red");
}

void AutonomousSelector::runSelectedAutonomous() {
    // Set brake modes before running autonomous
    driveLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveLeftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveRightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    driveRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);


    switch (currentRoutine) {
        case AutonomousRoutine::Four_Ring_Ring_Rush:
            disruptRingRush(allianceColorBlue);
            break;
        case AutonomousRoutine::Three_Ring_Mogo_Rush:
            newMogoRush(allianceColorBlue);
            break;
        case AutonomousRoutine::Regional_Solo_AWP_Mogo_Side:
            simpleMogo(allianceColorBlue);
            break;
        case AutonomousRoutine::Two_Ring_Safe_Mogo:
            verySimpleMogo(allianceColorBlue);
            break;
        case AutonomousRoutine::Two_Ring_Safe_Ring:
            simpleRing(allianceColorBlue);
            break;
        case AutonomousRoutine::Skills:
            allianceColorBlue = false;
            skills();
            break;
        case AutonomousRoutine::Disabled:
            break;
        default:
            break;
    }
}

void initializeSelector() {
    selector = AutonomousSelector::getInstance();
    
    // Register button callbacks for the LCD using correct PROS API
    pros::lcd::register_btn0_cb([]() {
        if(selector) selector->previousRoutine(); 
    });
    
    pros::lcd::register_btn1_cb([]() {
        if(selector) selector->toggleAllianceColor();
    });

    pros::lcd::register_btn2_cb([]() {
        if(selector) selector->nextRoutine();
    });
}