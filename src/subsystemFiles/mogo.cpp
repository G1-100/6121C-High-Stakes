#include "main.h"

using namespace std;
auto mogoTrigger = pros::E_CONTROLLER_DIGITAL_L1;
long buttonPressedTime = 0;
int RELEASEDELAY = 500;

void clampMogo(bool active) {
    mogoClamp.toggle();
}

// Driver Control Functions
void setMogoMotors() {
    if (mogoClamp.is_extended()) { // clamped
        if (master.get_digital_new_press(mogoTrigger)) {
            buttonPressedTime = pros::millis();
        }
        if (master.get_digital(mogoTrigger) && pros::millis() - buttonPressedTime > RELEASEDELAY) { // if button is held for x seconds
            mogoClamp.toggle(); // unclamp mogo
        }
    } else { // unclamped
        if (master.get_digital_new_press(mogoTrigger)) {
            mogoClamp.toggle(); // clamp mogo
            
        }
    }
    
}