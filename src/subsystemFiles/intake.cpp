#include "main.h"
#include "pros/misc.h"

// Helper functions
void setIntakeVelocity(int velocity) {
    // Sets the intake motor velocity directly using PROS motor control
    // Positive velocity = intake in, Negative velocity = intake out
    // velocity: target velocity in RPM (-600 to 600)
    intake.move_velocity(velocity);
}

// Driver Control Functions
void setIntakeMotors() {
    // Function handles intake control during driver control period
    // Uses velocity-based control for consistent intake power
    // Controls:
    //   - R2: Intake inward (collect game elements) at 600 RPM
    //   - R1: Outtake/reverse (release game elements) at -600 RPM
    //   - Neither pressed: Stop intake (0 RPM)
    
    int intakeVelocity = 0;  // Default state: stopped
    
    // Priority system: if both buttons pressed, R2 takes priority
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
        intakeVelocity = INTAKE_VELOCITY;     // Full speed intake
    } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
        intakeVelocity = -INTAKE_VELOCITY;    // Full speed outtake
    }
    // If neither button is pressed, intakeVelocity remains 0
    
    // Apply the calculated velocity to the intake motor
    setIntakeVelocity(intakeVelocity);
}
