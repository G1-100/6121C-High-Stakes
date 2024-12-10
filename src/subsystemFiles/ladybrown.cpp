#include "main.h"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystemHeaders/globals.hpp"
#include "main.h"
#include <string>

double RESTANGLE = 0; // actual -30
double STOP1 = 15 + 2; // angle of stopping point 1 actual -10
double STOP2 = 160 - 40; // angle of stop 2 - 130

// THESE ARE CURRENTLY UNUSED
double REST = 0;
double PROPPED = 1;
double EXTENDED = 2;
int LBState = REST;

bool LBLoopActive = false;


/**
 * @brief extend ladybrown to stopping point 1
 * 
 */
void LBExtend(int point) {
    double GOALANGLE;
    double power;
    if (point == 1) {
        GOALANGLE = STOP1;
        power = 70;
    } else if (point == 2) {
        GOALANGLE = STOP2;
        power = 127;
    }
    
    double curAngle = LBRotation.get_position() * -1 / 100.0;
    std::cout << "Extending to point " << point << ", Goal Angle: " << GOALANGLE << "\n";
    
    ladybrown.move(power);
      
    while (abs(GOALANGLE - curAngle) > 3) { // ends once above goal angle
        //std::cout << ladybrown.get_power() << "\n";
        curAngle = LBRotation.get_position() * -1 / 100.0;
        std::cout << "Current Angle: " << curAngle << "\n";
        if (curAngle > GOALANGLE) {
            ladybrown.move(-power);
        } else {
            ladybrown.move(power);
        }
        std::cout << curAngle << "\n";
        pros::delay(10);
    }
    std::cout << "Reached Goal Angle: " << curAngle << "\n";
    ladybrown.move(0); // stop once done
    if (point == 1) {
        LBState = PROPPED;
    } else if (point == 2) {
        LBState = EXTENDED;
    }
    
}

void LBReset() {
    ladybrown.move(100);
    pros::delay(2500);
    LBRotation.set_position(-12000);
    ladybrown.move(0);
    LBState = EXTENDED;
}

/**
 * @brief Retracts ladybrown to rest angle
 * 
 */
void LBRetract() {
    ladybrown.move(-127); // move beyond stopping point 2
    pros::delay(1000);
    ladybrown.move(0);
    LBState = REST;
    LBRotation.reset_position();
}

/**
 * @brief main ladybrown task loop
 * 
 */
void LBLoop() {
    LBLoopActive = true;
    ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    //LBRotation.reset();
    while (true) {
        ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
       //std::cout << std::to_string(LBRotation.get_position() / 100.0) << "\n";
       //std::cout << std::to_string(LBRotation.get_position() / 100.0) << "\n";
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) { // IMPORTANT: must be new_press
            double curAngle = LBRotation.get_position() * -1 / 100.0;
            //std::cout << "Button L2 pressed, Current Angle: " << curAngle << "\n";

            if (curAngle < STOP1 - 5) { // at stopping point 1
                std::cout << "At rest, extending to point 1\n";
                LBExtend(1); // go to stopping point 2
            } else if (curAngle < STOP2 - 5) { // at stopping point 2
                std::cout << "At stopping point 2, retracting to rest\n";
                LBExtend(2); // go to rest
            } else { // at rest
                std::cout << "At EXTENDED, going to rest\n";
                LBRetract(); // go to stopping point 1
            }
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
            LBReset();
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
            LBRetract();
        }
        pros::delay(20);
        ladybrown.move(10);
    }
}