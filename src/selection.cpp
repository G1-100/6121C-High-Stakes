/*#include "selection.hpp"
#include "pros/misc.h"
#include "pros/rtos.hpp"

// Initialize static member
AutonomousSelector* AutonomousSelector::instance = nullptr;

// Implementation of the singleton constructor
AutonomousSelector::AutonomousSelector() : currentRoutine(AutonomousRoutine::DISABLED) {
    const char* names[] = {
        "Ring Blue", "Ring Red", "Mogo Blue", "Mogo Red",
        "Solo AWP Blue", "Solo AWP Red", "Rush Mogo Blue",
        "Rush Mogo Red", "Skills", "Disabled"
    };
    std::copy(std::begin(names), std::end(names), routineNames);
    
    // Initialize selector UI
    pros::lcd::initialize();
    updateDisplay();
}

// Implement the rest of the member functions
// ...rest of the implementation remains the same...

void AutonomousSelector::nextRoutine() {
    int next = static_cast<int>(currentRoutine) + 1;
    if (next >= 10) next = 0;
    currentRoutine = static_cast<AutonomousRoutine>(next);
    updateDisplay();
}

void AutonomousSelector::previousRoutine() {
    int prev = static_cast<int>(currentRoutine) - 1;
    if (prev < 0) prev = 9;
    currentRoutine = static_cast<AutonomousRoutine>(prev);
    updateDisplay();
}

void AutonomousSelector::updateDisplay() {
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
        case AutonomousRoutine::RING_BLUE:
            ringAuton(true);
            break;
        case AutonomousRoutine::RING_RED:
            ringAuton(false); 
            break;
        case AutonomousRoutine::MOGO_BLUE:
            mogoAuton(true);
            break;
        case AutonomousRoutine::MOGO_RED:
            mogoAuton(false);
            break;
        case AutonomousRoutine::SOLO_AWP_BLUE:
            soloAWPAuton(true);
            break;
        case AutonomousRoutine::SOLO_AWP_RED:
            soloAWPAuton(false);
            break;
        case AutonomousRoutine::RUSH_MOGO_BLUE:
            rushMogoAuton(true);
            break;
        case AutonomousRoutine::RUSH_MOGO_RED:
            rushMogoAuton(false);
            break;
        case AutonomousRoutine::SKILLS:
            skills();
            break;
        case AutonomousRoutine::DISABLED:
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
        if(selector) selector->updateDisplay();
    });

    pros::lcd::register_btn2_cb([]() {
        if(selector) selector->nextRoutine();
    });
}
*/