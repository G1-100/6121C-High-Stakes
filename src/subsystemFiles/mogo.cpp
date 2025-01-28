#include "main.h"

using namespace std;
auto mogoTrigger = pros::E_CONTROLLER_DIGITAL_L1;

void clampMogo(bool active) {
    mogoClamp.toggle();
}

// Driver Control Functions
void setMogoMotors() {
    if (master.get_digital(mogoTrigger)) { // button pressed and not ladybrown pressed
        mogoClamp.retract();
    } else {
        mogoClamp.extend();
    }
}