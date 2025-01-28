#include "main.h"
int hue = 0;

using namespace std;

bool ColorLoopActive = false;
bool colorUntilActivated = false;
double ambientColorDiff = -20.3; // TODO: NEEDS TO BE TUNED AT COMPETITION
double ambientProximity = 31; // TODO: NEEDS TO BE TUNED AT COMPETITION
double ambientRed = 0;
double ambientBlue = 0;
double lastBlue = 0;
double lastRed = 0;
bool colorLoopStarted = false;
int ringsSeen = 0;
int colorUntilRings = 0;
bool safeScoring = false;
bool rightRingBeingSeen = false;
double prevHeading = 0;
long prevTime = 0;

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

void startColorUntil(int rings) {
    colorUntilActivated = true;
    colorUntilRings = rings;
    ringsSeen = 0;
}

void stopColorUntilFunction() {
    colorUntilActivated = false;
}

void doColorSort() {
        optical.set_led_pwm(100);
        double hue = optical.get_hue();
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

        const int PROXIMITYDIFFREQUIRED = 150;
        //std::cout << "RED: " << std::to_string(optical.get_rgb().red) << " BLUE: " << std::to_string(optical.get_rgb().blue) << "\n";
        //std::cout << "Proximity: " << optical.get_proximity() << " DIFF: " << currentColorDiff << "\n";
        if (ColorLoopActive) {
            if (curProximity - ambientProximity > PROXIMITYDIFFREQUIRED && (lastBlue != 0 || lastRed != 0) && !rightRingBeingSeen) { // ring detected
                std::cout << "PROXIMITY DETECTED: " << curProximity << "\n";
                if (currentColorDiff - ambientColorDiff > 5) { // blue ring
                    if (!allianceColorBlue) { // wrong color
                        cout << "BLUE DETECTED" << "\n";
                        master.rumble(". .");
                        wrongColorDetected = true;
                        setIntake(127);
                        long start = pros::millis();
                        while (optical.get_proximity() > ambientProximity + 10 && pros::millis() - start < 500) {
                            pros::delay(10);
                        }
                        setIntake(-127);
                        pros::delay(150);
                        setIntake(127);
                        wrongColorDetected = false;
                    } else { // right color
                        rightRingBeingSeen = true;
                        if (colorUntilActivated) { // intaking until that color
                            ringsSeen++;
                            if (ringsSeen >= colorUntilRings) {
                                intake.move(-127);
                                pros::delay(30);
                                intake.move(0);
                                colorUntilActivated = false;
                            } else if (safeScoring) { // wait until not scoring
                                cout << (IMU.get_heading() - prevHeading) / (pros::millis() - prevTime) << "\n";
                                while ((IMU.get_heading() - prevHeading) / (pros::millis() - prevTime) > 0.5) { // if angleChange / timeChange aka slope > 0.5
                                    setIntake(0);
                                }
                                setIntake(127);
                            }
                        }
                    }
                } else if (currentColorDiff - ambientColorDiff < -5) { // alliance blue and its a red ring
                    if (allianceColorBlue)  {
                        wrongColorDetected = true;
                        master.rumble(". .");
                        cout << "RED DETECTED" << "\n";
                        setIntake(127);
                        long start = pros::millis();
                        while (optical.get_proximity() > ambientProximity + 10 && pros::millis() - start < 500) {
                            pros::delay(10);
                        }
                        setIntake(-127);
                        pros::delay(150);
                        setIntake(127);
                        wrongColorDetected = false;
                    } else { // right color
                        rightRingBeingSeen = true;
                        if (colorUntilActivated) { // intaking until that color
                            ringsSeen++;
                            if (ringsSeen >= colorUntilRings) {
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
            //cout <<"HI COLOR" << "\n";
            lastBlue = blue_component;
            lastRed = red_component;
        }
        
        if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_B)) {
            ColorLoopActive = !ColorLoopActive;
        }
}

void colorSortLoop() {
    //colorLoopStarted = true;
    while (true) {
        if (LBState != PROPPED) {
            //cout <<"HI" << "\n";
            doColorSort();
        }
        prevHeading = IMU.get_heading();
        prevTime = pros::millis();
        pros::delay(10);
    }
}

void intakeUntilColor() { // TASK ONLY
    intake.move(90);
    double hue = optical.get_hue();
    if (allianceColorBlue) {
        while (hue < 180 && !colorUntilActivated) {
            //cout << "HUE: " + to_string(hue) << "\n";
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    } else {
        while (hue > 35 && !colorUntilActivated) {
            hue = optical.get_hue();
            setIntake(80);
            pros::delay(10);
        }
    }
    if (!colorUntilActivated) {
        setIntake(-127);
        pros::delay(200);
        setIntake(0);
    }
    
}