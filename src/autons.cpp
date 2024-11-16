#include "main.h"
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
lemlib::Pose leftAutonRedPos(-50,24,270);
lemlib::Pose rightAutonBluePos(50,24,90);

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

void simpleSkills() {
  chassis.setPose(-55, 0, 90); // starts at middle of red alliance line
  chassis.turnToHeading(-5, 3000); // turn towards mogo
  chassis.waitUntilDone();
  pros::delay(1000);
  chassis.moveToPoint(-48, -24 - 4, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  mogoClamp.toggle();
  pros::delay(1000);
  setIntake(127);
  chassis.turnToHeading(180, 3000);
  chassis.waitUntilDone();
  pros::delay(1000);
  chassis.moveToPoint(-48, -60 - 4, 3000);
  chassis.waitUntilDone();
  chassis.turnToPoint(-60, -48, 3000);
  chassis.waitUntilDone();
  chassis.moveToPoint(-60, -48, 3000);
  chassis.waitUntilDone();
  pros::delay(2000);
  chassis.turnToPoint(24, -48, 3000);
  chassis.waitUntilDone();
  chassis.moveToPoint(24, -48, 3000);
  chassis.waitUntilDone();
  
  
}

void skills() {
  chassis.setPose(-51.5f, 0, 90); // starts at middle of red alliance line
  chassis.turnToHeading(-5, 3000); // turn towards mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(-48, -24, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  // Piston clamp
  mogoClamp.toggle();
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

// Failsafe
/*
void simpleMogoAuton(bool isBlue) {
    int sgn;
    if (isBlue) {
        sgn = 1;
    } else {
        sgn = -1;
    }
    chassis.setPose(55 * sgn, -20, 90 * sgn);
    //pros::delay(500);
    chassis.turnToHeading(80 * sgn, 3000);
    chassis.waitUntilDone();
    //cout <<"hi" << "\n";
    //pros::delay(500);
    chassis.moveToPoint(20 * sgn, -24 - 4, 3000, {.forwards = false}); // move to mogo
    chassis.waitUntil(30);
    mogoClamp.toggle();
    chassis.waitUntilDone();
    pros::delay(500);
    setIntake(127);
    chassis.turnToPoint((24 - 2) * sgn, -52, 3000); // turn to two stack
    chassis.waitUntilDone();
    pros::delay(500);
    intake.move(127);
    chassis.moveToPoint((24 - 2) * sgn, -52, 3000, {.maxSpeed = 60}); // move to two stack
    chassis.waitUntilDone();
    pros::delay(1000);
    intake.move(0);
    chassis.moveToPoint(24 * sgn, -36, 3000, {.forwards = false}); // move back
    chassis.waitUntilDone();
    chassis.turnToPoint((-47) * sgn, -72 + 20, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint((47 - 3) * sgn, -72 + 20, 3000, {.forwards = false});
    chassis.waitUntilDone();
    mogoClamp.toggle();
    chassis.turnToHeading(75, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(35 * sgn, -48 + 5, 3000, {.forwards = false});
    // chassis.turnToPoint(8 * sgn, 0, 3000);
    // chassis.waitUntilDone();
    // chassis.moveToPoint((14 - 3) * sgn, -22 + 1, 3000);
    // chassis.waitUntilDone();
}
*//*
void soloAWPAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  //clampMogo(false);
  chassis.setPose(60 * sgn, 24, 90 * sgn);
  chassis.moveToPoint(29 * sgn, 24, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  clampMogo(true);
  pros::delay(500);
  chassis.turnToPoint(3 * sgn, 43, 3000); // turn to two stacks
  pros::delay(300);
  setIntake(127);
  chassis.waitUntilDone();
  chassis.moveToPoint(3 * sgn, 44, 3000); // move to two stacks
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
  // chassis.waitUntil(10);
  // LBExtend(2); // extend ladybrown to touch ladder
  chassis.waitUntilDone();

}
*/

void soloAWPAutonTunedLMSD(bool isBlue) {
  int sgn=isBlue?1:-1;
  chassis.setPose(50,24,90 * sgn); // Set position for left auton
  mogoClamp.toggle();
  //chassis.turnToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false}, false);
  chassis.moveToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  chassis.waitUntil(17);
  // Mogo piston activate
  mogoClamp.toggle();
  chassis.waitUntilDone();
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to bottom right rings of stack
  //chassis.turnToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5), 3000,{},false);
  chassis.moveToPoint((TSBRR.x + 6 - 3) * sgn, (TSBRR.y - 5), 3000, {.minSpeed = 60},false);
  chassis.swingToHeading(360, isBlue?lemlib::DriveSide::RIGHT:lemlib::DriveSide::LEFT,3000,{},false); // swing to intake first stack
  chassis.moveToPoint((TSBRR.x + 5)*sgn, (TSBRR.y+15),3000,{},false);
  pros::delay(200);
  chassis.turnToPoint((TSTRR.x+24)*sgn,TSTRR.y,3000,{.forwards=false},false);
  chassis.moveToPoint((TSTRR.x+24)*sgn,TSTRR.y,3000,{.forwards=false});
  chassis.waitUntil(2);
  mogoClamp.toggle();
  chassis.waitUntilDone();
  pros::delay(100);
  setIntake(0);
  // chassis.turnToPoint(46*sgn, -2, 3000,{.forwards=false},false);
  chassis.moveToPoint(46*sgn, -3, 3000,{.forwards=false});
  chassis.waitUntilDone();
  chassis.swingToHeading(-90 * sgn, isBlue?lemlib::DriveSide::RIGHT:lemlib::DriveSide::LEFT,3000); // swing to intake ring
  chassis.waitUntilDone();
  setIntake(127);
}
//Auton Development based on Barcbots (11101B)
//Preload one ring
void ringAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  chassis.setPose(rightAutonBluePos.x * sgn, rightAutonBluePos.y, 90 * sgn); // Set position for left auton
  //mogoClamp.toggle();
  //chassis.turnToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false}, false);
  chassis.moveToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  //chassis.waitUntil(17 + 2);
  // Mogo piston activate
  chassis.waitUntilDone();
  mogoClamp.toggle();
  //pros::delay(3000);
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to bottom right rings of stack
  //chassis.turnToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5), 3000,{},false);
  chassis.moveToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5 - 2), 3000, {.minSpeed = 60},false);
  chassis.swingToHeading(-90 * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT,3000); // swing to intake first stack
  chassis.waitUntilDone();
  pros::delay(200);
  chassis.moveToPoint((24 - 9) * sgn, 37 - 5,3000,{.forwards=false, .minSpeed=70},false); // move back
  chassis.moveToPoint((18) * sgn, 48 - 3.5, 3000);
  chassis.waitUntilDone();
  pros::delay(550);
  setIntake(-127);
  chassis.moveToPoint((18) * sgn, 48 - 8.5 + 1.5, 1000);
  chassis.waitUntilDone();
  mogoClamp.toggle(); // clamp to compact robot
  // Turn to and move to top right rings of stack
  chassis.turnToPoint((TSTRR.x-6 - 6) * sgn, TSTRR.y - 2, 3000, {}, false);
  setIntake(127);
  chassis.moveToPoint((TSTRR.x-4 + 3.5) * sgn,TSTRR.y - 4 - 3, 3000,{.minSpeed = 80},false);
  chassis.swingToHeading((-90 - 5) * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT,3000); // swing to intake first stack
  chassis.waitUntilDone();
  //setIntake(0);
  pros::delay(300);
  setIntake(0);
  chassis.moveToPoint((TSTRR.x+10 + 30)*sgn,TSTRR.y - 25,3000,{.forwards=false, .minSpeed = 70}); // move back to wall
  chassis.waitUntil(8);
  setIntake(0);
  chassis.waitUntilDone();
  mogoClamp.toggle();
  chassis.turnToPoint((46 + 2)*sgn, 2, 3000,{.forwards=false},false); // turn to alliance wall stake
  chassis.moveToPoint((46 + 2)*sgn, 2, 3000,{.forwards=false, .minSpeed = 80}); // move to alliance wall stake
  chassis.waitUntilDone();
  chassis.turnToHeading(-90 * sgn,1000); // turn to AWS
  chassis.waitUntilDone();
  chassis.moveToPoint((48 + 6)*sgn, -5 + 2, 3000, {.forwards = false});
  chassis.waitUntilDone();
  setIntake(127);
  pros::delay(400);
  setIntake(0);
  chassis.moveToPoint(20*sgn, -3, 3000);
  // move to alliance wall stake
  //chassis.moveToPose(72 * sgn, 0 , -90, 3000, {.forwards = false}, false);
  //chassis.moveToPoint(72 * sgn,0,3000,{},false);
  //setIntake(127); // score ring on alliance wall stake
  //chassis.moveToPoint(21 * sgn, 0, 3000); // touch ladder
  //chassis.waitUntilDone();

}

void ringAutonSimpler(bool isBlue) {
  int sgn=isBlue?1:-1;
  chassis.setPose(rightAutonBluePos.x * sgn, rightAutonBluePos.y, 90 * sgn); // Set position for left auton
  //mogoClamp.toggle();
  //chassis.turnToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false}, false);
  chassis.moveToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  //chassis.waitUntil(17 + 2);
  // Mogo piston activate
  chassis.waitUntilDone();
  mogoClamp.toggle();
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to bottom right rings of stack
  //chassis.turnToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5), 3000,{},false);
  chassis.moveToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5 - 2), 3000, {.minSpeed = 60},false);
  chassis.swingToHeading(-90 * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT,3000); // swing to intake first stack
  chassis.waitUntilDone();
  pros::delay(200);
  chassis.moveToPoint((24 - 9) * sgn, 37 - 5,3000,{.forwards=false, .minSpeed=70},false); // move back
  chassis.moveToPoint((18) * sgn, 48 - 3.5, 3000);
  chassis.waitUntilDone();
  pros::delay(550);
  setIntake(-127);
  chassis.moveToPoint((18) * sgn, 48 - 3.5 - 5, 1000);
  chassis.waitUntilDone();
  chassis.moveToPoint(21*sgn, -3, 3000); // move to ladder cone
  chassis.waitUntilDone();
  chassis.moveToPoint(21*sgn, -3, 3000); // move to ladder cone
  chassis.waitUntilDone();
}

void mogoAdvayAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  //mogoClamp.toggle();
	chassis.setPose(48*sgn,-60,90); //Starting Line
	chassis.moveToPoint((24 - 3)*sgn,-60,3000,{.forwards=false, .minSpeed = 90}); //Move next to ring stack
	chassis.waitUntil(27 - 2);
  chassis.cancelMotion();
	chassis.moveToPoint((5.5+4)*sgn,(-51.5-4 - 2),3000,{.forwards=false, .minSpeed = 80}); //Move to Mogo
	chassis.waitUntilDone();
	clampMogo(true);
  pros::delay(100);
	setIntake(127);
  pros::delay(500);
	chassis.turnToPoint((27 - 7)*sgn,-21,3000,{.forwards=false}); //Turn to mogo #2
	clampMogo(false); //unclamp mogo#1
	chassis.waitUntilDone();
	chassis.moveToPoint((27 - 6)*sgn,-21,3000,{.forwards=false});//Move to mogo #2
  chassis.waitUntil(10);
  setIntake(0);
	chassis.waitUntilDone();
  clampMogo(true);
  pros::delay(700);
	chassis.turnToPoint(25*sgn,-54,3000);//Turn to ring stack
	chassis.waitUntilDone();
  setIntake(127);
	chassis.moveToPoint(25*sgn,-48,3000);//Move to ring stack
	chassis.waitUntilDone();
  pros::delay(100);
  chassis.moveToPoint(25*sgn,(-55 + 20),3000, {.forwards = false});//Move backwards
	chassis.waitUntilDone();
  chassis.turnToPoint(24*sgn, 0, 3000);//Turn to ladder
  chassis.waitUntilDone();
  setIntake(0);
  chassis.moveToPoint(24 + 3 * sgn, 0, 3000);//Move to ladder
  chassis.waitUntilDone();
  setIntake(0);
}

/*void mogoAuton(bool isBlue) { // SAFE
  int sgn = isBlue?1:-1;
  chassis.setPose(60 * sgn, -15, 90 * sgn); // starts at lower half of alliance starting line
  chassis.turnToPoint(72 * sgn, 0, 3000); // turn to wall stake
  chassis.waitUntilDone();
  LBExtend(1); // activate lady brown
  chassis.moveToPoint(sgn * 64, -7, 3000); // move to wall stake
  LBExtend(2);
  chassis.waitUntilDone();
  chassis.moveToPose(sgn * 18, -26, 80 * sgn, 3000, {.forwards = false}); // boomerang move to mogo
  LBRetract(); // deactivate lady brown
  clampMogo(true); // clamp mogo
  chassis.turnToPoint(24 * sgn, -48, 3000); // turn to two stack
  chassis.waitUntilDone();
  setIntake(127);
  chassis.moveToPoint(24 * sgn, -52, 3000); // move to two stack
  chassis.waitUntilDone();
  chassis.turnToPoint(8 * sgn, 0, 3000);
  chassis.waitUntilDone();
  LBExtend(2); // extend to ladder
  chassis.moveToPoint(14 * sgn, -22, 3000);
  chassis.waitUntilDone();
}*/

/*void rushMogoAuton(bool isBlue) {
    int sgn = isBlue?1:-1;
    chassis.setPose(50 * sgn, -60, 90 * sgn); // starts at bottom of alliance starting line
    chassis.moveToPose(4 * sgn, -52, 135 * sgn, 3000, {.forwards = false, .minSpeed = 100}); // move to middle goal
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    chassis.moveToPoint(12 * sgn, -55, 3000); // move away from middle
    setIntake(127);
    chassis.waitUntilDone();
    chassis.turnToHeading(0, 3000); // turn to face upward
    chassis.waitUntilDone();
    mogoClamp.toggle(); // declamp mogo
    chassis.moveToPoint(12 * sgn, -32, 3000);
    chassis.waitUntilDone();
    chassis.turnToPoint(24 * sgn, -24, 3000, {.forwards = false}); // turn to clamp 2nd mogo
    chassis.waitUntilDone();
    chassis.moveToPoint((24 + 3) * sgn, -24 + 3, 3000, {.forwards = false}); // turn to clamp 2nd mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp 2nd mogo
    chassis.turnToPoint(24 * sgn, -48, 3000); // turn to two stack
    chassis.waitUntilDone();
    chassis.moveToPoint((24) * sgn, -48 - 3, 3000); // move to two stack
    chassis.waitUntilDone();
}*/