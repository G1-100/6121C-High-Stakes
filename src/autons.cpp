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

ASSET(skills1_txt);

int sgn(double num) {
    if (num >= 0) return 1;
    else return -1;
}

void moveMax(double dist, int timeout) {
  lemlib::Pose lastPose = chassis.getPose();
  lemlib::Pose curPose = chassis.getPose();
  double distTravelled = 0;
  long startMillis = pros::millis();
  while (distTravelled < fabs(dist) && pros::millis() - startMillis < timeout) {
    chassis.tank(127 * sgn(dist), 127 * sgn(dist));
    curPose = chassis.getPose();
    distTravelled += curPose.distance(lastPose);
    lastPose = chassis.getPose();
    pros::delay(10);
  }
  cout << "dist: " + to_string(fabs(dist)) << "\n";
  cout << "distraveled: " + to_string(distTravelled) << "\n";
  setDrive(0, 0);
}

void skills() {
  chassis.setPose(-51.5f, 0, 90); // starts at middle of red alliance line
  chassis.turnToHeading(-5, 3000); // turn towards mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, -24, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  // TODO: Piston clamp here
  setIntake(127);
  chassis.follow(skills1_txt, 10, 5000);
  chassis.waitUntilDone();
  chassis.turnToPoint(0, -59, 3000); // turn to ring near wall stake
  chassis.waitUntilDone();
  // TODO: Change to intake to ladybrown
  chassis.moveToPoint(0, -59, 3000); // move to ring near wall stake
  chassis.waitUntilDone();
  chassis.turnToHeading(180, 3000); // turn to wall stake
  chassis.waitUntilDone();
  // TODO: score ring on wall stake
  chassis.moveToPoint(0, -48, 3000, {.forwards = false}); // move back a little
  chassis.waitUntilDone();
  chassis.moveToPoint(-58.5, -48, 3000); // collect two bottom-left rings
  chassis.waitUntilDone();
  chassis.turnToPoint(-48, -60, 3000); // turn to last bottom-left ring
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, -60, 3000); // move to last bottom-left ring
  chassis.waitUntilDone();
  chassis.turnToPoint(-72, -65, 3000, {.forwards = false}); // robot-back turn to corner
  chassis.waitUntilDone();
  chassis.moveToPoint(-72, -65, 3000, {.forwards = false}); // back into corner
  chassis.waitUntilDone();
  // TODO: unclamp mogo
  chassis.moveToPoint(-48, -60, 3000); // move to last bottom-left ring
  chassis.waitUntilDone();
  chassis.turnToHeading(180, 3000); // turn to next mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, 24, 3000, {.minSpeed = 90}); // move to next mogo - FAST
  chassis.waitUntilDone();
  // TODO: clamp mogo
  chassis.turnToHeading(90, 3000);
  chassis.waitUntilDone();
  chassis.moveToPoint(-24, 24, 3000); // move to ring outside ladder
  chassis.waitUntilDone();
  chassis.turnToPoint(0, 0, 3000); // turn to middle
  chassis.waitUntilDone();
  chassis.moveToPoint(0, 0, 3000); // turn to middle
  chassis.waitUntilDone();
  chassis.moveToPoint(-24, 24, 3000, {.forwards = false}); // move back
  chassis.waitUntilDone();
  chassis.turnToHeading(90, 3000); // turn to top ring
  chassis.waitUntilDone();
  chassis.moveToPoint(-24, 48, 3000); // move to top ring
  chassis.waitUntilDone();
  chassis.turnToHeading(-90, 3000); // turn to two rings in row
  chassis.waitUntilDone();
  chassis.moveToPoint(-60, 48, 3000); // intake two rings
  chassis.waitUntilDone();
  chassis.turnToPoint(-48, 60, 3000); // turn to top ring
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, 60, 3000); // move to top ring
  chassis.waitUntilDone();
  chassis.turnToPoint(-72, 67, 3000, {.forwards = false}); // back turn to corner
  chassis.waitUntilDone();
  chassis.moveToPoint(-72, 67, 3000, {.forwards = false}); // back into corner
  chassis.waitUntilDone();
  // TODO : unclamp mogo
  chassis.moveToPoint(0, 60, 3000); // move to middle ring
  chassis.waitUntilDone();
  chassis.turnToHeading(0, 3000); // turn to wall stake
  chassis.waitUntilDone();
  // TODO: activate wall stake mech
  chassis.moveToPoint(0, 72, 3000); // move to wall stake
  chassis.waitUntilDone();
  chassis.moveToPoint(0, 48, 3000, {.forwards = false}); // move back a little
  chassis.waitUntilDone();
  chassis.turnToPoint(22, 26, 3000); // turn to ring
  chassis.waitUntilDone();
  chassis.moveToPoint(22, 26, 3000); // move to ring
  chassis.waitUntilDone();
  setIntake(0);
  chassis.turnToPoint(52, -4, 3000, {.forwards = false}); // back turn to mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(52, -4, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  // TODO: clamp mogo
  setIntake(127);
  chassis.turnToPoint(48, 60, 3000); // turn to two two-stack rings
  chassis.waitUntilDone();
  chassis.moveToPoint(48, 60, 3000); // move to two two-stacks
  chassis.waitUntilDone();
  chassis.moveToPoint(48, 36, 3000); // move back a lot
  chassis.waitUntilDone();
  chassis.turnToPoint(60, 48, 3000); // turn to third two-stack
  chassis.waitUntilDone();
  chassis.moveToPoint(60, 48, 3000); // move and intake third two-stack
  chassis.waitUntilDone();
  chassis.moveToPoint(48, 36, 3000, {.forwards = false}); // move back a bit
  chassis.waitUntilDone();
  chassis.turnToPoint(24, 48, 3000); // turn to last top ring
  chassis.waitUntilDone();
  chassis.moveToPoint(24, 48, 3000); // move and intake last top ring
  chassis.waitUntilDone();
  // TODO: the rest of skills starting from 44 seconds in vid




  
}

void leftAuton() {

}

void rightAuton() {

}