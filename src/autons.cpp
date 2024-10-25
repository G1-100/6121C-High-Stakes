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
lemlib::Pose ULM(-24,24); // Upper Left (Red) Side Mobile Goal
lemlib::Pose LLM(-24,-24); // Lower Left (Red) Side Mobile Goal
lemlib::Pose LC(0,-48); // Lower Center Mobile Goal
lemlib::Pose URM(24,24); // Upper Right (Blue) Side Mobile Goal
lemlib::Pose LBM(24,-24); // Lower Blue Side Mobile Goal
// Scoring Corners (4):
lemlib::Pose TLC(-60,60); // Top Left (Negative Red) Corner
lemlib::Pose PRC(-60,-60); // Bottom Left (Positive Red) Corner
lemlib::Pose BRC(60,-60); // Bottom Right (Positive Blue) Corner
lemlib::Pose TRC(60,60); // Top Right (Negative Blue) Corner
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
// Ladder Corners
lemlib::Pose LTC(0,26); // Ladder: Top Corner Center Line
lemlib::Pose LLC(-26,0); // Ladder: Left Corner
lemlib::Pose LBC(0,-26); // Ladder: Bottom Corner
lemlib::Pose LRC(26,0); // Ladder: Right Corner
// Ladder Beams
lemlib::Pose LTLB(-13,13); // Ladder: Top Left Beam (LTC-LLC)
lemlib::Pose LTRB(13,13); // Ladder: Top Right Beam (LTC-LRC)
lemlib::Pose LBLB(-13,-13); // Ladder: Bottom Left Beam (LBC-LLC)
lemlib::Pose LBRB(13,-13); // Ladder: Bottom Right Beam (LBC-LRC)
// STARTING POSITIONS
lemlib::Pose leftAutonRedPos(-54,34,90);
lemlib::Pose rightAutonBluePos(54,34,270);

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
  // Piston clamp
  clampMogo(true);
  setIntake(127);
  chassis.follow(skills1_txt, 10, 5000);
  chassis.waitUntilDone();
  chassis.turnToPoint(0, -59, 3000); // turn to ring near wall stake
  chassis.waitUntilDone();
  chassis.moveToPoint(0, -59, 3000); // move to ring near wall stake
  // ladybrown prop up
  LBExtend(1);
  chassis.waitUntilDone();
  chassis.turnToHeading(180, 3000); // turn to wall stake
  chassis.waitUntilDone();
  // score ring on wall stake
  LBExtend(2);
  chassis.moveToPoint(0, -48, 3000, {.forwards = false}); // move back a little
  chassis.waitUntilDone();
  // retract ladybrown
  LBRetract();
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
  // unclamp mogo
  clampMogo(false);
  chassis.moveToPoint(-48, -60, 3000); // move to last bottom-left ring
  chassis.waitUntilDone();
  chassis.turnToHeading(180, 3000); // turn to next mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, 24, 3000, {.minSpeed = 90}); // move to next mogo - FAST
  chassis.waitUntilDone();
  // clamp mogo
  clampMogo(true);
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
  // unclamp mogo
  clampMogo(false);
  chassis.moveToPoint(0, 60, 3000); // move to middle ring
  chassis.waitUntil(15);
  // prop up ladybrown
  LBExtend(1);
  chassis.waitUntilDone();
  chassis.turnToHeading(0, 3000); // turn to wall stake
  chassis.waitUntilDone();
  // activate wall stake mech
  chassis.moveToPoint(0, 72, 3000); // move to wall stake
  LBExtend(2);
  chassis.waitUntilDone();
  chassis.moveToPoint(0, 48, 3000, {.forwards = false}); // move back a little
  chassis.waitUntilDone();
  chassis.turnToPoint(22, 26, 3000); // turn to ring
  //retract ladybrown
  LBRetract();
  chassis.waitUntilDone();
  chassis.moveToPoint(22, 26, 3000); // move to ring
  chassis.waitUntilDone();
  setIntake(0);
  chassis.turnToPoint(52, -4, 3000, {.forwards = false}); // back turn to mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(52, -4, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  // clamp mogo
  clampMogo(true);
  setIntake(127); // intake touching ring
  chassis.turnToPoint(48, 60, 3000); // turn to two two-stack rings
  chassis.waitUntilDone();
  // lift intake
  intakeLift.set_value(true);

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
  // lower intake
  intakeLift.set_value(false);
  chassis.turnToPoint(24, 48, 3000); // turn to last top ring
  chassis.waitUntilDone();
  chassis.moveToPoint(24, 48, 3000); // move and intake last top ring
  chassis.waitUntilDone();
  // chassis.turnToPoint(48, 0, 3000, {.forwards = false});
  // chassis.waitUntilDone();
  chassis.moveToPose(48, 0, 0, 3000, {.forwards = false}); // boomerang move backwards to point
  chassis.waitUntilDone();
  chassis.turnToPoint(24, -24, 3000); // turn to bottom ring
  chassis.waitUntilDone();
  chassis.moveToPoint(24, -24, 3000); // intake ring
  chassis.waitUntilDone();
  chassis.turnToPoint(48, -48, 3000); // turn to two stack
  chassis.waitUntilDone();
  // lift intake
  intakeLift.set_value(true);
  chassis.moveToPoint(48, -48, 3000); // intake two stack
  chassis.waitUntilDone();
  chassis.turnToHeading(90 - 5, 3000); // turn horizontal to go back
  chassis.waitUntilDone();
  chassis.moveToPoint(24, -50, 3000, {.forwards = false}); // go back
  chassis.waitUntilDone();
  chassis.turnToPoint(58, -64, 3000); // turn to 2nd two stack
  chassis.waitUntilDone();
  setIntake(0);
  //lower intake
  intakeLift.set_value(false);
  // Activate doinker
  doinker.set_value(true);
  chassis.moveToPoint(58, -64, 3000); // move farther into corner
  chassis.waitUntilDone();
  chassis.turnToHeading(-45, 3000); // sweep out all rings
  chassis.waitUntilDone();
  // prop up ladybrown
  LBExtend(1);
  setIntake(127);
  // lift doinker
  doinker.set_value(false);
  chassis.moveToPoint(72, -72, 3000, {.forwards = false}); // move to corner
  chassis.waitUntilDone();
  // Unclamp mogo
  clampMogo(false);
  chassis.moveToPose(-38, 0, 0, 3000); // boomerang to wall stake
  chassis.waitUntilDone();
  chassis.turnToHeading(90, 3000); // turn to wall stake
  chassis.waitUntilDone();
  chassis.moveToPoint(72, 0, 3000); // move to wall stake
  // ladybrown extend
  LBExtend(2);
  chassis.waitUntilDone();
  chassis.moveToPoint(65, 0, 3000); // move back a little
  chassis.waitUntilDone();
  // ladybrown
  LBRetract();
  chassis.turnToHeading(180, 3000); // turn to mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(60, 28, 3000, {.forwards = false}); // move to mogo
  // Clamp mogo
  clampMogo(true);
  chassis.turnToHeading(5, 3000); // turn to corner
  chassis.waitUntilDone();
  // activate doinker
  doinker.set_value(false);
  chassis.moveToPoint(68, 66, 3000); // move to corner
  chassis.waitUntilDone();
  chassis.turnToHeading(-135, 3000); // sweep out rings
  chassis.waitUntilDone();
  // lift up doinker
  doinker.set_value(false);
  chassis.moveToPoint(72, 72, 3000, {.forwards = false}); // back into corner
  chassis.waitUntilDone();
  // Unclamp mogo
  clampMogo(false);
}

void ringAuton() {

}



void soloAWPAuton(bool isBlue) {
  int sgn;
  if (isBlue) {
    sgn = 1;
  } else {
    sgn = -1;
  }
  chassis.setPose(60 * sgn, 24, 90 * sgn);
  setIntake(127);
  chassis.moveToPoint(24 * sgn, 24, 3000, {.forwards = false}, false); // move to mogo
  clampMogo(true);
  chassis.turnToPoint(3 * sgn, 43, 3000); // turn to two stacks
  chassis.waitUntilDone();
  chassis.moveToPoint(3 * sgn, 43, 3000); // move to two stacks
  chassis.waitUntilDone();
  chassis.moveToPoint(24 * sgn, 24, 3000, {.forwards = false}); // move back a bunch
  chassis.waitUntilDone();
  chassis.turnToPoint(24 * sgn, 48, 3000); // turn to 2nd two stack
  chassis.waitUntilDone();
  chassis.moveToPoint(24 * sgn, 48, 3000); // move to 2nd two stack
  chassis.waitUntilDone();
  chassis.turnToPoint(3 * sgn, 50, 3000); // turn to 3rd two stack
  chassis.waitUntilDone();
  chassis.moveToPoint(3 * sgn, 50, 3000); // move to 3rd two stack
  chassis.waitUntilDone();
  chassis.moveToPose(42 * sgn, -13, -20 * sgn, 3000, {.forwards = false}); // move backwards to next mogo
  chassis.waitUntilDone();
  clampMogo(false);
  chassis.turnToPoint(24 * sgn, -24, 3000, {.forwards = false}); // turn to mogo
  chassis.waitUntilDone();
  
  chassis.moveToPoint(22 * sgn, -24, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  clampMogo(true);
  chassis.turnToPoint(24 * sgn, -48, 3000); // turn to two stack
  chassis.waitUntilDone();
  chassis.moveToPoint(24 * sgn, -50, 3000); // move to two stack
  chassis.waitUntilDone();
  chassis.turnToPoint(0 * sgn, 0, 3000); // turn to ladder
  chassis.waitUntilDone();
  chassis.moveToPoint(9 * sgn, -15, 3000); // move to ladder
  chassis.waitUntil(10);
  LBExtend(2); // extend ladybrown to touch ladder
  chassis.waitUntilDone();

}

//Auton Development based on Barcbots (11101B)
//Preload one ring
void ringAutonVirat(bool isBlue) {
  int sgn;
  if (isBlue) {
    sgn = -1;
  } else {
    sgn = 1;
  }
  chassis.setPose(leftAutonRedPos.x * sgn, leftAutonRedPos.y, leftAutonRedPos.theta * sgn); // Set position for left auton
  chassis.turnToPoint(ULM.x * sgn, ULM.y , 3000 ,{},false);
  chassis.moveToPoint(ULM.x * sgn, ULM.y ,3000,{},false); // Move to upper left mobile goal
  // Mogo piston activate
  clampMogo(true);
  // Turn to and move to bottom left rings of stack
  setIntake(127);
  chassis.turnToPoint(TSBLR.x * sgn, TSBLR.y, 3000,{},false);
  chassis.moveToPoint(TSBLR.x * sgn, TSBLR.y, 3000, {},false);
  // Turn to and move to top left rings of stack
  chassis.turnToPoint(TSTLR.x * sgn, TSTLR.y, 3000, {},false);
  chassis.moveToPoint(TSTLR.x * sgn,TSTLR.y,3000,{},false);
  // Turn to face ring and pick up
  chassis.turnToPoint(-24 * sgn,48,3000,{},false);
  // Face ring
  chassis.turnToPoint(-48 * sgn,0,3000,{},false);
  // Mogo piston relase
  clampMogo(false);
  // Lift flexwheel intake
  intakeLift.set_value(true);
  // move to point
  chassis.moveToPoint(-48 * sgn,0,3000,{},false);
  // Load ring into ladybrown mech
  LBExtend(1);
  chassis.turnToPoint(RAWS.x * sgn,RAWS.x,3000,{},false);
  setIntake(0);
  chassis.moveToPoint(RAWS.x * sgn,RAWS.y,3000,{},false);
  // Activate ladybrown mech
  LBExtend(2);

}
//Mirror of leftAutonRed() {...}
void rightAutonBlue() {
  chassis.setPose(rightAutonBluePos); // Set position for left auton
  chassis.turnToPoint(URM.x,URM.y,3000,{},false);
  chassis.moveToPoint(URM.x,URM.y,3000,{},false); // Move to upper left mobile goal
  // TODO: Mogo piston activate
  // Turn to and move to bottom left rings of stack
  setIntake(127);
  chassis.turnToPoint(TSBRR.x,TSBRR.y,3000,{},false);
  chassis.moveToPoint(TSBRR.x,TSBRR.y,3000,{},false);
  // Turn to and move to top left rings of stack
  chassis.turnToPoint(TSTRR.x,TSTRR.y,3000,{},false);
  chassis.moveToPoint(TSTRR.x,TSTRR.y,3000,{},false);
  // Turn to face ring and pick up
  chassis.turnToPoint(24,48,3000,{},false);
  // Face ring
  chassis.turnToPoint(48,0,3000,{},false);
  // TODO: Mogo piston relase
  // TODO: Lift flexwheel intake
  // move to point
  chassis.moveToPoint(48,0,3000,{},false);
  // TODO: Load ring into ladybrown mech
  chassis.turnToPoint(BAWS.x,BAWS.x,3000,{},false);
  chassis.moveToPoint(BAWS.x,BAWS.y,3000,{},false);
  // TODO: Activate ladybrown mech
}

void mogoAuton(bool isBlue) { // SAFE
  int sgn;
  if (isBlue) {
    sgn = 1;
  } else {
    sgn = -1;
  }
  chassis.setPose(60 * sgn, -15, 90 * sgn); // starts at lower half of alliance starting line
  chassis.turnToPoint(72 * sgn, 0, 3000); // turn to wall stake
  chassis.waitUntilDone();
  // prop up ladybrown
  LBExtend(1);
  chassis.moveToPoint(sgn * 64, -7, 3000); // move to wall stake
  LBExtend(2);
  chassis.waitUntilDone();
  chassis.moveToPose(sgn * 18, -26, 80 * sgn, 3000, {.forwards = false}); // boomerang move to mogo
  LBRetract();
  // clamp mogo
  clampMogo(true);
  chassis.turnToPoint(24 * sgn, -48, 3000); // turn to two stack
  chassis.waitUntilDone();
  setIntake(127);
  chassis.moveToPoint(24 * sgn, -52, 3000); // move to two stack
  chassis.waitUntilDone();
  chassis.turnToPoint(8 * sgn, 0, 3000);
  chassis.waitUntilDone();
  // extend ladybrown to touch ladder
  LBExtend(2);
  chassis.moveToPoint(14 * sgn, -22, 3000);
  chassis.waitUntilDone();
}