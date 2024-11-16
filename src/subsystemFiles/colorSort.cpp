#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;

void initColorSort() {
    ColorLoopActive = true;
    optical.set_led_pwm(100);
}

void doColorSort() {
        double hue = optical.get_hue();
        cout << "HUE: " + to_string(hue) << "\n";
        // if (ColorLoopActive) {
        //     if (!allianceColorBlue && (hue > 80 && hue < 130)) { // alliance red and its blue
        //     setIntake(127);
        //     pros::delay(180);
        //     setIntake(-127);
        //     pros::delay(100);
        //     }
        //     else if (allianceColorBlue && hue < 30) { // alliance blue and its red
        //         //cout << "RED DETECTED" << "\n";
        //         setIntake(127);
        //         pros::delay(180);
        //         setIntake(-127);
        //         pros::delay(100);
        //     }
        // }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    while (true) {
        doColorSort();
    }
}

void intakeUntilColor() { // TASK ONLY
    double hue = optical.get_hue();
    if (allianceColorBlue) {
        while (hue > 30) {
            setIntake(127);
            pros::delay(10);
        }
    } else {
        while (hue < 80) {
            setIntake(127);
            pros::delay(10);
        }
    }
    setIntake(0);
}
