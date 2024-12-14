#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;
bool stopColorUntil = false;


void initColorSort() {
    ColorLoopActive = false;
    optical.set_led_pwm(100);
    double ambientHue = 40;
}

void activateColorSort() {
    ColorLoopActive = true;
}
void stopColorUntilFunction() {
    stopColorUntil = true;
}

void doColorSort() {
        double hue = optical.get_hue();
        double red_component = optical.get_rgb().red;
        double blue_component = optical.get_rgb().blue;
        //std::cout << "RED: " << std::to_string(optical.get_rgb().red) << " BLUE: " << std::to_string(optical.get_rgb().blue) << "\n";
        if (ColorLoopActive) {
            if (!allianceColorBlue && blue_component > 150 && red_component < 200) { // alliance red and its blue
                cout << "BLUE DETECTED" << "\n";
                setIntake(127);
                pros::delay(20);
                setIntake(-127);
                pros::delay(40);
                setIntake(127);
            }
            else if (allianceColorBlue && red_component > 140 && blue_component < 100) { // alliance blue and its red
                cout << "RED DETECTED" << "\n";
                setIntake(127);
                pros::delay(60);
                setIntake(-127);
                pros::delay(40);
                setIntake(127);
            }
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    ColorLoopActive = true;
    while (true) {
        doColorSort();
        
        pros::delay(10);
    }
}

void intakeUntilColor() { // TASK ONLY
    intake.move(90);
    double hue = optical.get_hue();
    if (allianceColorBlue) {
        while (hue < 50 && !stopColorUntil) {
            //cout << "HUE: " + to_string(hue) << "\n";
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    } else {
        while (hue > 35 && !stopColorUntil) {
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    }
    if (!stopColorUntil) {
        setIntake(-127);
        pros::delay(200);
        setIntake(0);
    }
    
}