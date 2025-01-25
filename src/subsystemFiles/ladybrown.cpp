#include "main.h"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystemHeaders/globals.hpp"
#include "main.h"
#include <string>

double RESTANGLE = 0; // actual -30
double STOP1 = 41.57 - 0.5; // 42.57
double STOP1_5 = STOP1 + 45;
double STOP2 = 170 + 20; // angle of stop 2 - 130
double STOP3 = 250;

double REST = 0;
double PROPPED = 1;
double EXTENDED = 2;
double FULLEXTENDED = 3;
double SEMIEXTENDED = 1.5;
double LBState = REST;

double LBAutonGoal = REST;
int prevLBAutonGoal = REST;
bool calledLBReset = false;

bool LBLoopActive = false;
long pressTime = 0;
long totalPressTime = 0;
bool lastPressed = false;

bool intakeUnstuckActivated = false;

int intakeStuckTime = 0;


/**
 * ONLY supposed to be used when intaking full mogo and hooks get caught
 */
void doIntakeUnstuck() {
    if (fabs(intake.get_actual_velocity()) < 0.5 && fabs(intake.get_voltage()) > 2000 && LBState == REST) { // if intake is stuck
        if (intakeStuckTime == 0) {
            intakeStuckTime = pros::millis();
        } else if (pros::millis() - intakeStuckTime > 500) {
            master.rumble("-"); // short rumble to notify driver
            double intakePower = intake.get_power();
            intake.move(-127);
            pros::delay(400);
            intake.move(127);
            intakeStuckTime = 0;
        }
        
    }
}

void doLBAmbientAdjust(double curAngle) {
    if (LBState == PROPPED) {
        if (STOP1 - curAngle > 1) { // if below stopping point by more than 1 degree
            ladybrown.move(25);
        } else if (STOP1 - curAngle < -1) { // if above stopping point by more than 1 degree
            ladybrown.move(-5);
        } else {
            ladybrown.move(10);
        }
    } else if (LBState == SEMIEXTENDED) {
        if (STOP1_5 - curAngle > 5) { // if below stopping point by more than 1 degree
            ladybrown.move(25);
        } else if (STOP1_5 - curAngle < -5) { // if above stopping point by more than 1 degree
            ladybrown.move(-5);
        } else {
            ladybrown.move(10);
        }
    } else if (LBState == EXTENDED) {
        if (STOP2 - curAngle > 5) { // if below stopping point by more than 1 degree
            ladybrown.move(5);
        } else if (STOP2 - curAngle < -5) { // if above stopping point by more than 1 degree
            ladybrown.move(-30 - 15);
        } else {
            ladybrown.move(-10);
        }
    }
}


/**
 * @brief extend ladybrown to stopping point 1
 * 
 */
void LBExtend(double point) {
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
    } else if (point == 1.5) {
        GOALANGLE = STOP1_5;
        power = 70;
        negPower = -5;
        angleChange = STOP1_5 - STOP1;
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
    } else if (point == 1.5) {
        LBState = SEMIEXTENDED;
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

void ChangeLBState(double goal) {
    LBAutonGoal = goal;
}

/**
 * @brief Changes the ladybrown to a certain state
 * @param goal the goal to change to
 */
void ChangeLBAuton(double goal) {
    if (goal == REST) {
        LBRetract();
    } else if (goal == PROPPED) {
        LBExtend(1);
    } else if (goal == EXTENDED) {
        LBExtend(2);
    } else if (goal == FULLEXTENDED) {
        LBExtend(3);
    } else if (goal == SEMIEXTENDED) {
        LBExtend(1.5);
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
        //ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        double curAngle = LBRotation.get_position() / 100.0;
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) { // IMPORTANT: must be new_press
            if (!lastPressed) { // just pressed
                pressTime = pros::millis();
                totalPressTime = pros::millis();
            }
            if (pros::millis() - pressTime > 750) { // held for 0.75 seconds
                LBRetract();
                pressTime = pros::millis();
            }
            lastPressed = true;
        } else {
            if (lastPressed) {
                if (pros::millis() - totalPressTime > 600) { // held for 0.6 seconds
                    LBRetract();
                } else { // pressed for normal logic
                    
                    //std::cout << "Button L2 pressed, Current Angle: " << curAngle << "\n";
                    if (curAngle < STOP1 - 5) { // at stopping point 1
                        std::cout << "At rest, extending to point 1\n";
                        LBExtend(1); // go to stopping point 2
                    } else if (curAngle < STOP2 - 5 && LBState != EXTENDED) { // at PROPPED
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
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_DOWN)) {
            LBExtend(3);
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
            set_drive(-8, 1500); // move back
            chassis.waitUntilDone();
            LBExtend(3);
        }
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_UP)) {
            std::cout << "DOWN BUTTON PRESSED" << "\n";
            LBExtend(1.5);
            //LBState = SEMIEXTENDED;
        }
        if (LBAutonGoal != prevLBAutonGoal) { // interact with LB in auton mode
            ChangeLBAuton(LBAutonGoal);
        }
        if (calledLBReset) {
            LBRetract();
            calledLBReset = false;
        }
        prevLBAutonGoal = LBAutonGoal;
        doLBAmbientAdjust(curAngle);
        if (intakeUnstuckActivated) {
            doIntakeUnstuck();
        }
        doIntakeUnstuck();
        pros::delay(20);
    }
}


void TwoRingLBMacro() {
    intake.move(110);
    startColorUntil(1);
    LBExtend(2);
    set_drive(-8, 1000);
    chassis.waitUntilDone();
    set_drive(8, 1000);
    LBRetract();
    chassis.waitUntilDone();
    LBExtend(2);
    set_drive(-8, 1000);
    chassis.waitUntilDone();
    stopColorUntilFunction();
}