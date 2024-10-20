#include "main.h"
#include "autons.hpp"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "lemlib/api.hpp"

const int DRIVE_SPEED = 110; // This is 110/127 (around 87% of max speed).  We don't suggest making this 127.
                             // If this is 127 and the robot tries to heading correct, it's only correcting by
                             // making one side slower.  When this is 87%, it's correcting by making one side
                             // faster and one side slower, giving better heading correction.
const int TURN_SPEED  = 90;
const int SWING_SPEED = 90;

using namespace std;

int sgn(double num) {
    if (num >= 0) return 1;
    else return -1;
}

void moveMax(double dist, int timeout, double slowAt) {
  lemlib::Pose lastPose = chassis.getPose();
  lemlib::Pose curPose = chassis.getPose();
  double distTravelled = 0;
  long startMillis = pros::millis();
  while (distTravelled < fabs(dist) && pros::millis() - startMillis < timeout) {
    chassis.tank(127 * sgn(dist), 127 * sgn(dist));
    if (slowAt != -1 && distTravelled > slowAt) {
      setIntake(90);
      cout << "hi" << "\n";
    }
    curPose = chassis.getPose();
    distTravelled += curPose.distance(lastPose);
    lastPose = chassis.getPose();
    
    pros::delay(10);
  }
  cout << "dist: " + to_string(fabs(dist)) << "\n";
  cout << "distravveled: " + to_string(distTravelled) << "\n";
  setDrive(0, 0);
}

void leftAuton() {

}

void rightAuton() {

}