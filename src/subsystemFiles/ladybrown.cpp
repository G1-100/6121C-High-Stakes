#include "main.h"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystemHeaders/globals.hpp"
#include "main.h"
#include <string>

const double RESTANGLE = 0; // actual -30
const double STOP1 = 20; // angle of stopping point 1 actual -10
const double STOP2 = 160; // angle of stop 2 - 130

// THESE ARE CURRENTLY UNUSED
const double REST = 0;
const double PROPPED = 1;
const double EXTENDED = 2;
double LBState = 0;

bool LBLoopActive = false;


/**
 * @brief extend ladybrown to stopping point 1
 * 
 */
void LBExtend(int point) {
    double GOALANGLE;
    if (point == 1) {
        GOALANGLE = STOP1;
    } else if (point == 2) {
        GOALANGLE = STOP2;
    }
    double curAngle = LBRotation.get_position() / 100.0;
    ladybrown.move(127);
      
    while (GOALANGLE - curAngle > 0) { // ends once above goal angle
        curAngle = LBRotation.get_position() / 100.0;
        pros::delay(10);
    }
    ladybrown.move(0); // stop once done
    
}


/**
 * @brief Retracts ladybrown to rest angle
 * 
 */
void LBRetract() {
    double curAngle = LBRotation.get_position() / 100.0;
    ladybrown.move(-127); // move beyond stopping point 2
    while (curAngle >= RESTANGLE) { // while it has not gone above the start angle, this is weird but works
        curAngle = LBRotation.get_position() / 100.0;
        pros::delay(20);
    }
    ladybrown.move(0);
}

/**
 * @brief main ladybrown task loop
 * 
 */
void LBLoop() {
    LBLoopActive = true;
    ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    while (true) {
        pros::lcd::print(2, "Angle: %f", LBRotation.get_position() / 100.0);
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) { // IMPORTANT: must be new_press
            double curAngle = LBRotation.get_position() / 100.0;
            if (curAngle < STOP1 + 5 && curAngle > STOP1 - 5) { // at stopping point 1
                LBExtend(2); // go to stopping point 2
            } else if (curAngle > STOP2 - 5) { // at stopping point 2
                LBRetract(); // go to rest
            } else { // at rest
                LBExtend(1); // go to stopping point 1
            }
        }
        pros::delay(20);
    }
}