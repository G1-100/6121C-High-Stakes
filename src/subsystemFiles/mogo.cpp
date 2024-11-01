#include "main.h"

using namespace std;

void clampMogo(bool active) {
    mogoClamp.toggle();
}

// Driver Control Functions
void setMogoMotors() {
    
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
        mogoClamp.toggle();
    }
}
