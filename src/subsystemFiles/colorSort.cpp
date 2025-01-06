#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;
bool stopColorUntil = false;
double ambientColorDiff = -5.15; // TODO: NEEDS TO BE TUNED AT COMPETITION
double ambientProximity = 29; // TODO: NEEDS TO BE TUNED AT COMPETITION
double ambientRed = 0;
double ambientBlue = 0;
bool colorLoopStarted = false;

void initColorSort() {
    optical.set_led_pwm(100);
    double ambientHue = 50;
    pros::Task color_task(colorSortLoop);
    optical.set_integration_time(10);
}

void activateColorSort() {
    //ColorLoopActive = true;
    //ambientRed = optical.get_rgb().red;
    //ambientBlue = optical.get_rgb().blue;
    ambientColorDiff = ambientBlue - ambientRed; // diff is BLUE - RED
    ambientProximity = optical.get_proximity();
}


void stopColorUntilFunction() {
    stopColorUntil = true;
}

void doColorSort() {
    optical.set_led_pwm(100);
        double hue = optical.get_hue();
        double red_component = optical.get_rgb().red;
        double blue_component = optical.get_rgb().blue;
        double currentColorDiff = blue_component - red_component;
        //std::cout << "RED: " << std::to_string(optical.get_rgb().red) << " BLUE: " << std::to_string(optical.get_rgb().blue) << "\n";
        //std::cout << "Proximity: " << optical.get_proximity() << "\n";
        if (ColorLoopActive) {
            if (!allianceColorBlue && currentColorDiff - ambientColorDiff > 10) { // alliance red and its 100 more blue than before
                cout << "BLUE DETECTED" << "\n";
                wrongColorDetected = true;
                setIntake(127);
                long start = pros::millis();
                while (optical.get_proximity() > ambientProximity + 15 && pros::millis() - start < 500) {
                    pros::delay(10);
                }
                setIntake(-127);
                pros::delay(150);
                setIntake(127);
                wrongColorDetected = false;
            }
            else if (allianceColorBlue && currentColorDiff - ambientColorDiff < -10) { // alliance blue and its 100 more red than before
                wrongColorDetected = true;
                cout << "RED DETECTED" << "\n";
                setIntake(127);
                long start = pros::millis();
                while (optical.get_proximity() > ambientProximity + 5 && pros::millis() - start < 500) {
                    pros::delay(10);
                }
                setIntake(-127);
                pros::delay(150);
                setIntake(127);
                wrongColorDetected = false;
            }
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            //ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    //colorLoopStarted = true;
    while (true) {
        if (LBState == REST) {
            doColorSort();
        }
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