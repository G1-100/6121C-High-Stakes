#include "main.h"
int hue = 0;

bool ColorLoopActive = false;

void initColorSort() {
    ColorLoopActive = true;
    optical.set_led_pwm(100);
}

void doColorSort() {    
        hue = optical.get_hue();
        if (!allianceColorBlue && (hue < 300 && hue > 240)) {
            setIntake(127);
            pros::delay(100);
            setIntake(-127);
        }
        else if (allianceColorBlue && ((hue < 30 && hue > 0) || (hue > 330 && hue < 360))) {
            setIntake(127);
            pros::delay(100);
            setIntake(-127);
        }
        pros::delay(10);
}
