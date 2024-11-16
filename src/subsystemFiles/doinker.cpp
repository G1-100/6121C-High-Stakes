#include "main.h"

using namespace std;

void toggleDoinker(bool active) {
    doinker.toggle();
}

void setDoinker() {
    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)) {
        doinker.toggle();
    }
}