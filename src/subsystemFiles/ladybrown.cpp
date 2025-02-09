#include "main.h"
#include "pros/llemu.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "subsystemHeaders/globals.hpp"
#include "main.h"
#include <string>

enum LadybrownState {
    REST = 0,
    PROPPED = 44,
    READY = 190,
    EXTENDED = 250,
//    CURRENT = LadybrownState::REST,
};

double currentAngle = 0;
double targetAngle = 0;

bool LBLoopActive = false;

ez::PID lbPID{1, 0.003, 4, 100, "Ladybrown"};
// kP, kl, kD, P_start_i, name

void LBLoop() {
    ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
    while (true) {

    }
}