#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;
bool stopColorUntil = false;
double colorDiff = 100;


void initColorSort() {
    ColorLoopActive = false;
    optical.set_led_pwm(100);
    double ambientHue = 50;
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
            if (!allianceColorBlue && blue_component > 200 && red_component < 210) { // alliance red and its blue
                cout << "BLUE DETECTED" << "\n";
                setIntake(127);
                pros::delay(30);
                setIntake(-127);
                pros::delay(40);
                setIntake(127);
            }
            else if (allianceColorBlue && red_component > 220 && blue_component < 160) { // alliance blue and its red
                cout << "RED DETECTED" << "\n";
                setIntake(127);
                pros::delay(50);
                setIntake(-127);
                pros::delay(40);
                setIntake(127);
            }
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) {
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
        while (hue < 180 && !stopColorUntil) {
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