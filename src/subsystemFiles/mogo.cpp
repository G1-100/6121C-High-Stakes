#include "main.h"

// Driver Control Functions
void setMogoMotors() {
    static bool toggleState = false;    // Tracks if clamp is open/closed
    
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        toggleState = !toggleState;
        clampMogo(toggleState);  // Uses helper from globals.cpp to control both pistons
    }
}
