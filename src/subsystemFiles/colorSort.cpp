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
        //cout << "HUE: " + to_string(hue) << "\n";
        if (ColorLoopActive) {
            if (!allianceColorBlue && (hue > 80)) { // alliance red and its blue
                cout << "BLUE DETECTED" << "\n";
                setIntake(127);
                pros::delay(100);
                setIntake(-127);
                pros::delay(100);
            }
            else if (allianceColorBlue && hue < 30) { // alliance blue and its red
                cout << "RED DETECTED" << "\n";
                setIntake(127);
                pros::delay(100);
                setIntake(-127);
                pros::delay(100);
            }
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    while (true) {
        setIntake(127);
        doColorSort();
        
        pros::delay(10);
    }
}

void intakeUntilColor() { // TASK ONLY
    intake.move(90);
    double hue = optical.get_hue();
    if (allianceColorBlue) {
        while (hue < 60) {
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    } else {
        while (hue < 80) {
            setIntake(127);
            pros::delay(10);
        }
    }
    setIntake(-127);
    pros::delay(150);
    setIntake(0);
}
