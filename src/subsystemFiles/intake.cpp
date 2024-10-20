#include "main.h"
#include "pros/misc.h"

// Helper functions
void setIntake(int power) {
    intake.move(power);
}

// Driver Control Functions

// bool intakeToggle = false;

void setIntakeMotors() {
    // R2 Trigger (back right) intake - hold R2 to power intake

    // int intakePower;
    // if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) == 1) {
    //     intakePower = 127;
    // } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) == 1) {
    //     intakePower = -90;
    // }
    
    int intakePower = 127 * (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2) - master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)); 
    setIntake(intakePower);
}
