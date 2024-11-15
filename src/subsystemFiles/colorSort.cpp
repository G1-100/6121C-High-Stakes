#include "main.h"
int hue = 0;

bool ColorLoopActive = false;
bool isBlue=false;

void colorSortLoop() {    
    ColorLoopActive = true;
    optical.set_led_pwm(100);
    while (true) {
        hue = optical.get_hue();
        if (!isBlue && (hue < 300 && hue > 240)) {
            setIntake(127);
            pros::delay(100);
            setIntake(-127);
        } 
        else if (isBlue && ((hue < 30 && hue > 0) || (hue > 330 && hue < 360))) {
            setIntake(127);
            pros::delay(100);
            setIntake(-127);
        }
        pros::delay(20);
    }
}