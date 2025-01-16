#include "main.h"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystemHeaders/globals.hpp"
#include "main.h"
#include <string>

double RESTANGLE = 0; // actual -30
double STOP1 = 46 + 0.1; // angle of stopping point 1 actual -10
double STOP2 = 170; // angle of stop 2 - 130
double STOP3 = 250;

double REST = 0;
double PROPPED = 1;
double EXTENDED = 2;
double FULLEXTENDED = 3;
int LBState = REST;

int LBAutonGoal = REST;
int prevLBAutonGoal = REST;
bool calledLBReset = false;

bool LBLoopActive = false;
long pressTime = 0;
bool lastPressed = false;

bool intakeUnstuckActivated = false;


/**
 * ONLY supposed to be used when intaking full mogo and hooks get caught
 */
void doIntakeUnstuck() {
    if (fabs(intake.get_actual_velocity()) < 2 && fabs(intake.get_voltage()) > 2000 && LBState == REST) { // if intake is stuck
        double intakePower = intake.get_power();
        intake.move(-127);
        pros::delay(100);
        intake.move(127);
    }
}


/**
 * @brief extend ladybrown to stopping point 1
 * 
 */
void LBExtend(int point) {
    double GOALANGLE;
    double power;
    double negPower;
    double angleChange;
    double iterationsRequired;
    if (point == 1) {
        GOALANGLE = STOP1;
        power = 70;
        negPower = 0;
        iterationsRequired = 40;
        angleChange = STOP1 - 0;
    } else if (point == 2) {
        GOALANGLE = STOP2;
        power = 100;
        negPower = -10;
        angleChange = STOP2 - STOP1;
        iterationsRequired = 1;
    } else if (point == 3) {
        GOALANGLE = STOP3;
        power = 90;
        negPower = -15;
        angleChange = STOP3 - STOP2;
        iterationsRequired = 1;
    }
    long startTime = pros::millis();
    double timeStayedGood = 0; // time stayed within range
    double curAngle = LBRotation.get_position() / 100.0;
    std::cout << "Extending to point " << point << ", Goal Angle: " << GOALANGLE << "\n";
    
    ladybrown.move(power);
      
    while ((abs(GOALANGLE - curAngle) > 3 || timeStayedGood < iterationsRequired) && pros::millis() - startTime < 2500) { // ends once above goal angle
        curAngle = LBRotation.get_position() / 100.0;
        //std::cout << "Current Angle: " << curAngle << "\n";
        if (curAngle > GOALANGLE) {
            ladybrown.move(negPower);
        } else {
            if (point == 1) {
                ladybrown.move(power * (abs(GOALANGLE - curAngle) / angleChange + 0.3));
            } else {
                ladybrown.move(power);
            }
        }
        if (abs(GOALANGLE - curAngle) < 3) {
            timeStayedGood += 1;
        } else {
            timeStayedGood = 0;
        }
        if (curAngle > GOALANGLE && point > 1) {
            break;
        }
        // if (ladybrown.get_efficiency() < 10 && pros::millis() - startTime > 500) { // not moving after 500 milliseconds
        //     break;
        // }
        pros::delay(10);
    }
    std::cout << "Reached Goal Angle: " << curAngle << "\n";
    ladybrown.move(0); // stop once done
    if (point == 1) {
        LBState = PROPPED;
    } else if (point == 2) {
        LBState = EXTENDED;
    } else if (point == 3) {
        LBState = FULLEXTENDED;
    }
    
}

void LBReset() {
    ladybrown.move(100);
    pros::delay(2500);
    ladybrown.move(0);
    LBState = EXTENDED;
}

/**
 * @brief Retracts ladybrown to rest angle
 * 
 */
void LBRetract() {
    ladybrown.move(-127); // move beyond stopping point 2
    //pros::delay(1000);
    pros::delay(200);
    while (fabs(ladybrown.get_actual_velocity()) > 1) {
        pros::delay(20);
    }
    ladybrown.move(0);
    LBState = REST;
    LBRotation.reset_position();
}

void ChangeLBState(int goal) {
    LBAutonGoal = goal;
}

/**
 * @brief Changes the ladybrown to a certain state
 * @param goal the goal to change to
 */
void ChangeLBAuton(int goal) {
    if (goal == REST) {
        LBRetract();
    } else if (goal == PROPPED) {
        LBExtend(1);
    } else if (goal == EXTENDED) {
        LBExtend(2);
    } else if (goal == FULLEXTENDED) {
        LBExtend(3);
    }
}

void callLBReset() {
    calledLBReset = true;
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
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) { // IMPORTANT: must be new_press
            if (!lastPressed) { // just pressed
                pressTime = pros::millis();
            }
            lastPressed = true;
        } else {
            if (lastPressed) {
                if (pros::millis() - pressTime > 750) { // held for 0.75 seconds
                    LBRetract();
                } else { // pressed for normal logic
                    double curAngle = LBRotation.get_position() / 100.0;
                    //std::cout << "Button L2 pressed, Current Angle: " << curAngle << "\n";
                    if (curAngle < STOP1 - 5) { // at stopping point 1
                        std::cout << "At rest, extending to point 1\n";
                        LBExtend(1); // go to stopping point 2
                    } else if (curAngle < STOP2 - 5 && LBState != EXTENDED) { // at stopping point 2
                        std::cout << "At stopping point 1, going to stopping point 2\n";
                        LBExtend(2); // go to rest
                    } else { // at rest
                        std::cout << "At EXTENDED, going to rest\n";
                        LBRetract(); // go to stopping point 1
                    }
                }
            }
            lastPressed = false;

        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
            LBExtend(3);
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
            std::cout << "DOWN BUTTON PRESSED" << "\n";
            LBRetract();
        }
        if (LBAutonGoal != prevLBAutonGoal) { // interact with LB in auton mode
            ChangeLBAuton(LBAutonGoal);
        }
        if (calledLBReset) {
            LBRetract();
            calledLBReset = false;
        }
        prevLBAutonGoal = LBAutonGoal;
        if (LBState == PROPPED) {
            ladybrown.move(10);
        } else if (LBState == EXTENDED) {
            ladybrown.move(-5);
        } else if (LBState == FULLEXTENDED) {
            ladybrown.move(-10);
        }
        if (intakeUnstuckActivated) {
            doIntakeUnstuck();
        }
        doIntakeUnstuck();
        pros::delay(20);
    }
}