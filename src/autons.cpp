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

// FIELD OBJECTS AND POSITIONS IN INCHES:
// Wall Stakes (4):
lemlib::Pose RAWS(-71,0); // Red Alliance Wall Stake
lemlib::Pose BAWS(71,0); // Blue Alliance Wall Stake
lemlib::Pose UWS(71,0); // Upper Wall Stake
lemlib::Pose LWS(-71,0); // Lower Wall Stake
// Mobile Goals (5)
lemlib::Pose URMogo(-24,24); // Upper Red Side Mobile Goal
lemlib::Pose LRMogo(-24,-24); // Lower Red Side Mobile Goal
lemlib::Pose LC(0,-48); // Lower Center Mobile Goal
lemlib::Pose UBMogo(24,24); // Upper Blue Side Mobile Goal
lemlib::Pose LBMogo(24,-24); // Lower Blue Side Mobile Goal
// Scoring Corners (4):
lemlib::Pose NRC(-60,60); // Negative Red Corner
lemlib::Pose PRC(-60,-60); // Positive Red Corner
lemlib::Pose NBC(60,-60); // Negative Blue Corner
lemlib::Pose PBC(60,60); // Positive Blue Corner
// RING POSITIONS:
// 2x2 Rings at center of field
lemlib::Pose CTLR(-3.5,3.5); // Center Top Left: Blue Ring
lemlib::Pose CTRR(3.5,3.5); // Center Top Right: Red Ring
lemlib::Pose CBLR(-3.5,-3.5); // Center Bottom Left: Blue Ring
lemlib::Pose CBRR(3.5,-3.5);// Center Bottom RIght: Red Ring
// 2x2 2 Ring Stacks on center line
lemlib::Pose TSTLR(-3.5,51.5); // Top Stack Top Left Rings: ↑:Blue ↓:Red
lemlib::Pose TSTRR(3.5,51.5); // Top Stack Top Right Rings: ↑:Red ↓:Blue
lemlib::Pose TSBLR(-3.5,44.5); // Top Stack Bottom Left Rings: ↑:Blue ↓:Red
lemlib::Pose TSBRR(3.5,44.5); // Top Stack Bottom Right Rings: ↑:Red ↓:Blue
// LADDER POSITIONS:
lemlib::Pose LTC(0,24); // Ladder: Top Corner
lemlib::Pose LLC(-24,0); // Ladder: Left Corner
lemlib::Pose LBC(0,-24); // Ladder: Bottom Corner
lemlib::Pose LRC(24,0); // Ladder: Right Corner



using namespace std;

int sgn(double num) {
    if (num >= 0) return 1;
    else return -1;
}

/*
  goal: move as fast as possible in a straight line, ignoring accuracy
 */
void moveMax(double dist, int timeout) {
  //these two variables used to track distance traveled
  lemlib::Pose lastPose = chassis.getPose();
  lemlib::Pose curPose = chassis.getPose();
  double distTravelled = 0;
  long startMillis = pros::millis(); // used to see how much time has elapsed
  while (distTravelled < fabs(dist) && pros::millis() - startMillis < timeout) { // if robot has traveled far enough or time has gone over the limit
    chassis.tank(127 * sgn(dist), 127 * sgn(dist)); // move at max speed, account for backwards
    curPose = chassis.getPose();
    distTravelled += curPose.distance(lastPose); // find distance between positions and add to distance traveled
    lastPose = chassis.getPose();
    pros::delay(10);
  }
  setDrive(0, 0); // stop moving
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

//Auton Development based on Barcbots (11101B)

void leftAutonVirat() {

}

void rightAuton() {

}