#include "main.h"

using namespace std;

bool ColorLoopActive = false;
bool colorUntilActivated = false;
double ambientColorDiff = -4; // TODO: NEEDS TO BE TUNED AT COMPETITION
double ambientProximity = 35; // TODO: NEEDS TO BE TUNED AT COMPETITION
bool colorLoopStarted = false;
int ringsSeen = 0;
int colorUntilRings = 0;
bool safeScoring = false;
bool rightRingBeingSeen = false;
double prevHeading = 0;
long prevTime = 0;


/**
 * Initialize color sort function
 */
void initColorSort() {
    optical.set_led_pwm(100); // turn brightness from optical sensor to max
    pros::Task color_task(colorSortLoop); // start color sort task
    optical.set_integration_time(10); // make optical sensor get measurements every 10 ms
}


/**
 * @param rings number of rings to intake until
 */
void startColorUntil(int rings) {
    colorUntilActivated = true;
    colorUntilRings = rings;
    ringsSeen = 0;
}

/**
 * in case you don't want to continue looking for the right ring
 */
void stopColorUntilFunction() {
    colorUntilActivated = false;
}

/**
 * Main color sort loop, features...
 * Autocalibration - calibrates proximity to lowest and calibrates color difference when proximity is normal
 * Color Sort - detects blue and red rings and flings them when wrong color
 * Color Until - intakes until a certain color is seen, then stops and keeps it in intake
 * Safe Scoring - waits until not turning to intake; NOTE: not tested yet, not needed
 * Proximity Detection - detects when ring is gone to reverse intake
 */
void doColorSort() {
        optical.set_led_pwm(100);
        double red_component = optical.get_rgb().red;
        double blue_component = optical.get_rgb().blue;
        double currentColorDiff = blue_component - red_component;
        double curProximity = optical.get_proximity();
        if (curProximity < ambientProximity) {
            ambientProximity = curProximity; // calibrate proximity
        }
        if (fabs(curProximity - ambientProximity) < 5) {
            ambientColorDiff = currentColorDiff;
        }

        const int PROXIMITYDIFFREQUIRED = 70;
       
        if (ColorLoopActive) {
            if (curProximity - ambientProximity > PROXIMITYDIFFREQUIRED && !rightRingBeingSeen) { // ring detected
                if (currentColorDiff - ambientColorDiff > 5) { // blue ring
                    if (!allianceColorBlue) { // wrong color
                        cout << "BLUE DETECTED" << "\n";
                        master.rumble(". .");
                        wrongColorDetected = true; // stop driver intake when color sorting
                        setIntake(127);
                        long start = pros::millis();
                        while (optical.get_proximity() > ambientProximity + 9 && pros::millis() - start < 500) { // fling ring after 500 ms or until undetected
                            intake.move(110);
                            pros::delay(10);
                        }
                        setIntake(-127);
                        pros::delay(150 + 50);
                        setIntake(127);
                        wrongColorDetected = false;
                    } else { // right color
                        if (colorUntilActivated && !rightRingBeingSeen) { // intaking until that color
                            ringsSeen++;
                            rightRingBeingSeen = true;
                            if (ringsSeen >= colorUntilRings) { // stop color until
                                intake.move(-127);
                                pros::delay(30);
                                intake.move(0);
                                colorUntilActivated = false;
                            } else if (safeScoring) { // wait until not scoring
                                while ((IMU.get_heading() - prevHeading) / (pros::millis() - prevTime) > 0.5) { // if angleChange / timeChange aka slope > 0.5
                                    setIntake(0);
                                    pros::delay(10);
                                }
                                setIntake(127);
                            }
                        }
                    }
                } else if (currentColorDiff - ambientColorDiff < -5) { // red ring
                    if (allianceColorBlue)  { // wrong color
                        wrongColorDetected = true; // stop driver intake
                        master.rumble(". .");
                        cout << "RED DETECTED" << "\n";
                        setIntake(127);
                        long start = pros::millis();
                        while (optical.get_proximity() > ambientProximity + 9 && pros::millis() - start < 500) { // wait until undetected or 500 ms to fling
                            intake.move(110);
                            pros::delay(10);
                        }
                        setIntake(-127);
                        pros::delay(150 + 50);
                        setIntake(127);
                        wrongColorDetected = false;
                    } else { // right color
                        if (colorUntilActivated && !rightRingBeingSeen) { // intaking until that color
                            rightRingBeingSeen = true;
                            ringsSeen++;
                            if (ringsSeen >= colorUntilRings) {
                                std::cout <<"right red seen" << "\n";
                                intake.move(-127);
                                pros::delay(30);
                                intake.move(0);
                                colorUntilActivated = false;
                            }
                        } else if (safeScoring) { // wait until not turning
                            cout << (IMU.get_heading() - prevHeading) / (pros::millis() - prevTime) << "\n";
                            while ((IMU.get_heading() - prevHeading) / (pros::millis() - prevTime) > 0.5) { // if angleChange / timeChange aka slope > 0.5
                                setIntake(0);
                            }
                            setIntake(127);
                        }
                        
                    }
                }
            } else {
                rightRingBeingSeen = false;
            }
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) { // deactivate color sort when B pressed
            ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    while (true) {
        if (LBState != PROPPED) { // don't run color sort when ladybrown is propped
            doColorSort();
        }
        prevHeading = IMU.get_heading();
        prevTime = pros::millis();
        pros::delay(10);
    }
}
