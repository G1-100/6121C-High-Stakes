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

/*
void skills() {
  chassis.setPose(-63, 0, 90); // starts at middle of red alliance line
  setIntake(127);
  chassis.moveToPoint(-48, 0, 3000); // move backward
  chassis.waitUntilDone();
  chassis.moveToPoint(-44.5, -24, 3000, {.forwards = false}); // move to mogo
  chassis.waitUntilDone();
  // Piston clamp
  mogoClamp.toggle();
  chassis.follow(skills1_txt, 10, 5000);
  chassis.waitUntilDone();
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
}*/

void LMSDSkills() {
    chassis.setPose(-63, 0, 90); // starts at middle of red alliance line
    setIntake(127); // score on alliance stake
    pros::delay(500);
    chassis.moveToPoint(-48, 0, 3000); // move away from alliance stake
    chassis.waitUntilDone();
    chassis.turnToPoint(-48 - 3, -23 + 5, 3000, {.forwards = false}); // move to mogo
    chassis.waitUntilDone();
    chassis.moveToPoint(-48 - 3, -23 + 5, 3000, {.forwards = false}); // move to mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    pros::delay(600);
    chassis.turnToPoint(-22, -24 + 3, 3000);
    chassis.waitUntilDone();
    chassis.follow(skills1_txt, 15, 3000); // pure pursuit two rings
    chassis.waitUntilDone();
    pros::delay(1000);
    // chassis.moveToPoint(-22 - 2, -24 + 3, 3000); // pickup ring outside ladder
    // chassis.waitUntilDone();
    // pros::delay(600);

    // chassis.moveToPoint(24, -48, 3000); // move to farther ring
    // chassis.waitUntilDone();
    // chassis.moveToPoint(24, -48, 3000); // move to farther ring
    // chassis.waitUntilDone();

    chassis.turnToPoint(-5 + 2, -55 + 2, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-5 + 2, -55 + 2, 3000); // pickup ring next to wall stake
    chassis.waitUntilDone();
    pros::delay(1000);
    chassis.turnToPoint(-26, -46 + 3, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-26, -46 + 3, 3000); // pickup ring next to wall stake
    chassis.waitUntilDone();
    pros::delay(600);
    chassis.moveToPoint(-24,-48,3000, {.forwards=false});
    chassis.waitUntilDone();
    chassis.turnToPoint(-59 + 2, -48, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-59 + 2, -48, 3000); // collect two bottom-left rings
    chassis.waitUntilDone();
    pros::delay(1000);
    chassis.turnToPoint(-50 + 3, -58, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-50 + 3, -58, 3000); // turn to last bottom-left ring
    chassis.waitUntilDone();
    pros::delay(600);
    chassis.turnToPoint(-57.2, -57.2, 1000, {.forwards = false});
    chassis.waitUntilDone();
    chassis.moveToPoint(-57.2, -57.2, 500, {.forwards = false}); // move to corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-60,-60, 500, {.forwards=false}); // look to center
    chassis.waitUntilDone();
    mogoClamp.toggle();
    pros::delay(600);
    chassis.moveToPoint(-48+2, -48, 3000); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-48,22,3000, {.forwards=false});
    chassis.waitUntilDone();
    setIntake(0);
    chassis.moveToPoint(-48,22 + 4, 3000, {.forwards=false}); // move to mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    pros::delay(600);
    chassis.turnToPoint(-20, 24 + 2, 3000);
    chassis.waitUntilDone();
    setIntake(127);
    chassis.moveToPoint(-20, 24 + 2, 3000); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(700);
    chassis.moveToPoint(-24, 24, 3000, {.forwards = false}); // move back
    chassis.waitUntilDone();
    chassis.turnToPoint(-3, 55, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-2 + 2, 59 + 2, 3000); // move to ring next to wall stake
    chassis.waitUntilDone();
    pros::delay(1200);
    chassis.turnToPoint(-26, 46 + 3, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-26, 50 - 1, 3000); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(700);
    chassis.moveToPoint(-24, 50 - 1, 3000, {.forwards=false});
    chassis.waitUntilDone();
    chassis.turnToPoint(-60 + 3, 50 + 2, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-60 + 3, 50 + 2, 3000); // move to two top-left rings
    chassis.waitUntilDone();
    pros::delay(1200);
    chassis.turnToPoint(-48 + 3, 63 - 3, 1500);
    chassis.waitUntilDone();
    chassis.moveToPoint(-48 + 3, 63 - 3, 3000); // move to top ring
    pros::delay(600);
    chassis.waitUntilDone();
    chassis.turnToPoint(-57.2, 57.2, 3000, {.forwards=false});
    chassis.waitUntilDone();
    chassis.moveToPoint(-57.2, 57.2, 3000, {.forwards=false}); // back turn to corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-59, 60, 500, {.forwards=false});
    chassis.waitUntilDone();
    chassis.moveToPoint(-59, 60, 500, {.forwards=false}); // move to corner
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    chassis.waitUntilDone();
    pros::delay(200);
    chassis.turnToPoint(-48, 48, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-48, 48, 3000); // back out of corner
    chassis.waitUntilDone();
    chassis.turnToPoint(48, 48, 3000);
    chassis.waitUntilDone();
    setIntake(0);
    chassis.moveToPoint(5, 48, 3000); // move to ladder-center
    chassis.waitUntilDone();
    setIntake(127);
    chassis.turnToPoint(48, 10, 3000); // turn to mogo
    chassis.waitUntilDone();
    chassis.moveToPoint(48 - 10, 10 - 10, 3000); // move to mogo and ring
    chassis.waitUntil(20);
    setIntake(0);
    chassis.waitUntilDone();
    chassis.turnToPoint(48, 0, 3000, {.forwards=false});
    chassis.waitUntilDone();
    chassis.moveToPoint(48, 0, 3000, {.forwards=false});
    chassis.waitUntilDone();
    // chassis.moveToPoint(48, 2, 3000, {.forwards=false}); // move back
    // chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    pros::delay(200);
    setIntake(127);
    chassis.turnToPoint(24 - 3, -24, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(24 - 3, -24, 3000); // move to ring
    chassis.waitUntilDone();
    pros::delay(600);
    chassis.turnToPoint(24, -48, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(24, -48, 3000); // move to ring
    chassis.waitUntilDone();
    pros::delay(600);
    pros::Task color_task(colorSortLoop);
    chassis.turnToPoint(60, -48, 3000);
    chassis.waitUntilDone();
    chassis.turnToPoint(60, -48, 3000);
    chassis.waitUntilDone();
    chassis.turnToPoint(60, -48, 3000, {.maxSpeed=40});
    chassis.waitUntilDone();
    chassis.moveToPoint(60, -48, 3000, {.maxSpeed=40});
    chassis.waitUntilDone();
    
    
    


    /*chassis.turnToPoint(36, 36, 3000, {.forwards=false, .maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(36, 36, 3000, {.forwards=false, .maxSpeed=60}); // move to other side
    chassis.waitUntilDone();
    chassis.turnToPoint(60, 48, 3000, {.forwards=false,.maxSpeed=60});
    chassis.waitUntilDone();
    mogoClamp.toggle(); // grab mogo
    chassis.turnToPoint(60, 60, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(60, 60, 3000, {.maxSpeed=90}); // move mogo into corner
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    pros::delay(100);
    chassis.moveToPoint(56, 42, 3000, {.forwards=false, .maxSpeed=90}); // move back
    chassis.waitUntilDone();
    chassis.moveToPoint(50,3,3000, {.forwards=false, .maxSpeed=60}); // move to mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    pros::delay(300);
    chassis.turnToPoint(22, 22, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(22, 22, 3000, {.maxSpeed=60}); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(300);
    chassis.moveToPoint(24, 24, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.turnToPoint(24, 46, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(24, 46, 3000, {.maxSpeed=60}); // move to top ring
    chassis.waitUntilDone();
    chassis.turnToPoint(36, 36, 3000, {.forwards=false,.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(36, 36, 3000, {.forwards=false,.maxSpeed=60}); // move back
    chassis.waitUntilDone();
    chassis.turnToPoint(2, 2, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(2, 2, 3000, {.maxSpeed=60}); // move to middle
    chassis.waitUntilDone();
    chassis.moveToPoint(22, -22, 3000, {.maxSpeed=60}); // grab ring outside ladder
    chassis.waitUntilDone();
    pros::delay(300);
    chassis.moveToPoint(24, -24, 3000, {.forwards=false, .maxSpeed=60}); // move forward
    chassis.waitUntilDone();
    chassis.turnToPoint(24, -46, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(24, -46, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    pros::delay(300);
    chassis.moveToPoint(24, -52, 3000, {.forwards=false, .maxSpeed=60});
    chassis.waitUntilDone();
    chassis.turnToPoint(57.5,-57.5,3000, {.forwards=false, .maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(57.5, -57.5, 3000, {.forwards=false, .maxSpeed=90}); // move to corner
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    pros::delay(300);
    chassis.turnToPoint(60, -48, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();
    chassis.moveToPoint(60, -48, 3000, {.maxSpeed=60});
    chassis.waitUntilDone();*/
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
  mogoClamp.toggle();
  chassis.moveToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  //chassis.waitUntil(17 + 2);
  // Mogo piston activate
  chassis.waitUntilDone();
  mogoClamp.toggle();
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to middle stacks of rings
  chassis.turnToPoint((TSBRR.x + 6) * sgn, (TSBRR.y - 5), 3000,{},false); // turn to rings
  chassis.follow(isBlue?ringRushBlue_txt:ringRushRed_txt, 15, 4000); // pure pursuit move while intaking rings
  chassis.waitUntilDone();
  pros::delay(2000); // wait a little

  chassis.moveToPoint((15) * sgn, 24, 3000, {.forwards=false}); // move back a lot
  chassis.waitUntilDone();

  chassis.turnToPoint((18) * sgn, 48 - 3.5, 3000); // turn to 3rd two stack
  chassis.waitUntilDone();
  chassis.moveToPoint((18) * sgn, 48 - 3.5, 3000); // move to 3rd two stack
  chassis.waitUntilDone();
  pros::delay(550);
  chassis.moveToPoint((18) * sgn, 48 - 8.5 + 1.5, 1000); // move back a little
  chassis.waitUntilDone();

  //TODO: FINISH REST LATER
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

}

void VexmenSoloAWP(bool isBlue) {
  int sgn=isBlue?1:-1;
  clampMogo(false);
  LBRetract();
  chassis.setPose(51*sgn,36,-90*sgn);
  chassis.moveToPoint(12*sgn,36,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  setIntake(127);
  chassis.turnToPoint(2.5*sgn,48-2.5,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(2.5*sgn,48-2.5,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  pros::delay(500);
  setIntake(0);
  chassis.moveToPoint((23)*sgn,24,3000,{.forwards=false});
  chassis.waitUntilDone();
  clampMogo(true);
  pros::delay(500);
  //LBExtend(1);
  setIntake(127);
  chassis.turnToPoint(24,48,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(24,50,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  pros::delay(200);
  setIntake(0);
  chassis.turnToPoint(5,67,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(5,57,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  //LBExtend(2);
  pros::delay(500);
  //LBRetract();
  chassis.turnToPoint(0,48,3000,{.forwards=false,.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(0,48,3000,{.forwards=false,.maxSpeed=60});
  chassis.waitUntilDone();
  setIntake(127);
  chassis.turnToPoint(63,48,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(63,48,3000,{.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.turnToPoint(10,10,3000,{.forwards=false,.maxSpeed=60});
  chassis.waitUntilDone();
  chassis.moveToPoint(10,10,3000,{.forwards=false,.maxSpeed=60});
  chassis.waitUntilDone();
}

void mogoAdvayAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  //mogoClamp.toggle();
	chassis.setPose(48*sgn,-60,90 * sgn); //Starting Line
	chassis.moveToPoint((21 + 2.5)*sgn,-60,3000,{.forwards=false, .minSpeed = 90}); //Move next to ring stack
	chassis.waitUntil(27 - 2);
  chassis.cancelMotion();
	chassis.moveToPoint((9.5)*sgn,(-51.5 - 3.5),3000,{.forwards=false, .minSpeed = 80}); //Move to Mogo
	chassis.waitUntilDone();
  // chassis.swingToHeading(90 * sgn, isBlue?lemlib::DriveSide::RIGHT:lemlib::DriveSide::LEFT, 3000);
  // chassis.waitUntilDone();
	clampMogo(true);
  pros::delay(100);
	setIntake(127);
  pros::delay(500);
	chassis.turnToPoint((27 - 7 - 3)*sgn,-21 + 2,3000,{.forwards=false}); //Turn to mogo #2
	clampMogo(false); //unclamp mogo#1
	chassis.waitUntilDone();
  //pros::Task colorTask(colorSortLoop);
	chassis.moveToPoint((27 - 6)*sgn,-21,3000,{.forwards=false});//Move to mogo #2
  chassis.waitUntil(10);
	chassis.waitUntilDone();
  clampMogo(true);
  pros::delay(700);
  setIntake(127);
	chassis.turnToPoint(25*sgn,-54,3000);//Turn to ring stack
	chassis.waitUntilDone();
  
	chassis.moveToPoint((21 + 2)*sgn,-48,3000, {.minSpeed = 60});//Move to ring stack
	chassis.waitUntilDone();
  pros::delay(600);
  chassis.moveToPoint(25*sgn,(-55 + 20),3000, {.forwards = false});//Move backwards
  setIntake(-127);
	chassis.waitUntilDone();
  setIntake(0);
  chassis.turnToHeading(-90 * sgn, 3000);
  chassis.waitUntilDone();
  setIntake(0);
  chassis.moveToPoint((27 + 15 + 5) * sgn, -40, 3000, {.forwards = false});//Move to ladder
  chassis.waitUntilDone();
  setIntake(0);
  pros::delay(300);
  mogoClamp.toggle();
  pros::delay(100);
  chassis.turnToPoint(24 * sgn, -24 - 4, 1500);
  chassis.waitUntilDone();
  chassis.moveToPoint(24 * sgn, -24 - 4, 2000);
  chassis.waitUntilDone();
  chassis.turnToPoint(0 * sgn, -48, 3000, {.forwards = false});
  chassis.waitUntilDone();

}

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
