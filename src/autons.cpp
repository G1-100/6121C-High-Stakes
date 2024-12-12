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

int sign(bool isBlue) {
    return isBlue?1:-1;
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
}
//Auton Development based on Barcbots (11101B)
//Preload one ring
void ringAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  chassis.setPose(rightAutonBluePos.x * sgn, rightAutonBluePos.y, 90 * sgn); // Set position for left auton
  //mogoClamp.toggle();
  chassis.moveToPoint((URM.x + 4) * sgn, URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  //chassis.waitUntil(17 + 2);
  // Mogo piston activate
  chassis.waitUntilDone();
  mogoClamp.toggle();
  pros::Task color_task(colorSortLoop); // Start color sorting task
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to middle stacks of rings
  chassis.turnToPoint((TSBRR.x) * sgn, (TSBRR.y - 5), 3000,{},false); // turn to rings
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
  mogoClamp.toggle(); // drop mogo

  //TODO: FINISH REST LATER
  chassis.moveToPoint((TSTRR.x+35)*sgn,TSTRR.y - 25,3000,{.forwards=false, .minSpeed = 70}); // move back to wall
  chassis.waitUntil(8);
  //setIntake(0);
  chassis.waitUntilDone();
  //mogoClamp.toggle();
  chassis.turnToPoint((40)*sgn, 2, 3000,{}, false); // turn to alliance wall stake
  intakeLift.toggle();
  chassis.moveToPoint((40)*sgn, 2, 3000,{.minSpeed = 80}); // move to two stack
  chassis.waitUntilDone();
  chassis.moveToPoint(20*sgn, -3, 3000);

}

void AceRingSideSoloAWP(bool isBlue) {
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
void MogoSideSoloAWP(bool isBlue) {
  lemlib::Pose preload(60,-27.5);
  lemlib::Pose allianceRing(48,0);
  int sgn=sign(isBlue);
  lemlib::Pose start(60,-27.5+(14.5/2),-90);
  intakeLift.toggle();
  intake.move(127);
  chassis.setPose(start.x*sgn,start.y,start.theta*sgn);
  //LBExtend(1); // prop up ladybrown as hard stop for ring
  chassis.turnToPoint((allianceRing.x + 6)*sgn,allianceRing.y,3000); // turn to two stack
  chassis.waitUntilDone();
  chassis.moveToPoint((allianceRing.x + 6)*sgn,(allianceRing.y - 9),3000, {.maxSpeed=30}); // move to two stack
  chassis.waitUntil(10);
  intakeLift.toggle(); // lift intake to get rings
  chassis.waitUntilDone();
  pros::Task colorUntil(intakeUntilColor); // start color sort
  chassis.moveToPoint((start.x)*sgn,(start.y),3000, {.forwards = false, .minSpeed = 50}); // move back to starting position
  chassis.waitUntilDone();
  //intake.move(0);
  chassis.turnToPoint(62 * sgn,-5 - 1,3000, {.minSpeed = 50}); // turn to AWS area
  chassis.waitUntilDone();
  chassis.moveToPoint(62 * sgn,-6 + 2,3000); // move to AWS area
  chassis.waitUntilDone();
  //LBRetract(); // retract ladybrown
  intake.move(0); // stop intake
  chassis.turnToHeading(-85 * sgn,3000, {.minSpeed = 50}); // turn to AWS
  chassis.waitUntilDone();
  //colorUntil.remove();
  chassis.moveToPoint(66 * sgn,-8 + 2,1000,{.forwards=false,.minSpeed=50}); // move to AWS
  chassis.waitUntil(2);
  intake.move(127); // score on AWS
  chassis.waitUntilDone();
  pros::delay(200);
  intake.move(0);
  chassis.moveToPoint(55 * sgn,-6 - 8,3000, {.minSpeed = 60}); // move toward ladder a little
  chassis.waitUntilDone();
  chassis.turnToPoint(32 * sgn,-30,3000,{.forwards=false, .minSpeed = 50}); // turn to mogo
  chassis.waitUntilDone();
  chassis.moveToPoint(32 * sgn,-30,3000,{.forwards=false, .minSpeed = 60, .earlyExitRange = 3}); // move to mogo
  chassis.waitUntilDone();
  mogoClamp.toggle(); // clamp mogo
  intake.move(127);
  chassis.turnToPoint(35 * sgn,-45 + 4,3000); // turn to two stack
  chassis.waitUntilDone();
  chassis.moveToPoint(35 * sgn,-45 + 4,3000, {.minSpeed = 60}); // move to two stack
  chassis.waitUntilDone();
  pros::Task sortColor(colorSortLoop); // start color sort
  pros::delay(700);
  chassis.turnToPoint(57 * sgn,-26 - 2,3000, {.minSpeed = 60, .earlyExitRange = 5}); // turn to preload
  chassis.waitUntilDone();
  chassis.moveToPoint(57 * sgn,-26 - 2,3000, {.minSpeed = 60}); // move to preload
  chassis.waitUntilDone();
  pros::delay(400);
  chassis.turnToPoint(18 * sgn,-18,3000); // turn to ladder
  chassis.waitUntilDone();
  sortColor.remove(); // stop color sort
  chassis.moveToPoint(18 * sgn,-18,3000, {.minSpeed = 80}); // move to ladder
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