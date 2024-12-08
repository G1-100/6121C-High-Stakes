#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;


void initColorSort() {
    ColorLoopActive = false;
    optical.set_led_pwm(100);
}

void activateColorSort() {
    ColorLoopActive = true;
}

void doColorSort() {
        double hue = optical.get_hue();
        //cout << "HUE: " + to_string(hue) << "\n";
        if (ColorLoopActive) {
            if (!allianceColorBlue && (hue > 40 + ambientHue)) { // alliance red and its blue
                //cout << "BLUE DETECTED" << "\n";
                setIntake(127);
                pros::delay(40);
                setIntake(-127);
                pros::delay(40);
            }
            else if (allianceColorBlue && hue < ambientHue - 10) { // alliance blue and its red
                cout << "RED DETECTED" << "\n";
                setIntake(127);
                pros::delay(40);
                setIntake(-127);
                pros::delay(40);
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
        while (hue < 50) {
            cout << "HUE: " + to_string(hue) << "\n";
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    } else {
        while (hue > 35) {
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    }
    setIntake(-127);
    pros::delay(50);
    setIntake(0);
}
