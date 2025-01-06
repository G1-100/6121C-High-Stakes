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

// ORIENT YOURSELF TO THE FIELD:
// The field is oriented with the red alliance on the left and the blue alliance on the right.
// The center mobile goal should be lower.
// This orientation makes a cartesian plane with the origin at the center of the field.

// FIELD OBJECTS AND POSITIONS IN INCHES:
// Wall Stakes (4):
lemlib::Pose RAWS(-71,0); // Red Alliance Wall Stake (-71,0)
lemlib::Pose BAWS(71,0); // Blue Alliance Wall Stake (71,0)
lemlib::Pose UWS(71,0); // Upper Wall Stake (71,0)
lemlib::Pose LWS(-71,0); // Lower Wall Stake (-71,0)
// Mobile Goals (5)
lemlib::Pose ULM(-24,24); // Upper Left (Red) Side Mobile Goal (-24,24)
lemlib::Pose LLM(-24,-24); // Lower Left (Red) Side Mobile Goal (-24,-24)
lemlib::Pose LC(0,-48); // Lower Center Mobile Goal (0,-48)
lemlib::Pose URM(24,24); // Upper Right (Blue) Side Mobile Goal (24,24)
lemlib::Pose LBM(24,-24); // Lower Blue Side Mobile Goal (24,-24)
// Scoring Corners (4):
lemlib::Pose TLC(-60,60); // Top Left (Negative Red) Corner (-60,60)
lemlib::Pose PRC(-60,-60); // Bottom Left (Positive Red) Corner (-60,-60)
lemlib::Pose BRC(60,-60); // Bottom Right (Positive Blue) Corner (60,-60)
lemlib::Pose TRC(60,60); // Top Right (Negative Blue) Corner (60,60)
// RING POSITIONS:
// 2x2 Rings at center of field
lemlib::Pose CTLR(-3.5,3.5); // Center Top Left: Blue Ring (-3.5,3.5)
lemlib::Pose CTRR(3.5,3.5); // Center Top Right: Red Ring (3.5,3.5)
lemlib::Pose CBLR(-3.5,-3.5); // Center Bottom Left: Blue Ring (-3.5,-3.5)
lemlib::Pose CBRR(3.5,-3.5);// Center Bottom RIght: Red Ring (3.5,-3.5)
// 2x2 2 Ring Stacks on center line
lemlib::Pose TSTLR(-3.5,51.5); // Top Stack Top Left Rings: ↑:Blue ↓:Red (-3.5,51.5)
lemlib::Pose TSTRR(3.5,51.5); // Top Stack Top Right Rings: ↑:Red ↓:Blue (3.5,51.5)
lemlib::Pose TSBLR(-3.5,44.5); // Top Stack Bottom Left Rings: ↑:Blue ↓:Red (-3.5,44.5)
lemlib::Pose TSBRR(3.5,44.5); // Top Stack Bottom Right Rings: ↑:Red ↓:Blue (3.5,44.5)
// LADDER POSITIONS:
// Ladder Corners
lemlib::Pose LTC(0,26); // Ladder: Top Corner Center Line (0,26)
lemlib::Pose LLC(-26,0); // Ladder: Left Corner (-26,0)
lemlib::Pose LBC(0,-26); // Ladder: Bottom Corner Center Line (0,-26)
lemlib::Pose LRC(26,0); // Ladder: Right Corner (26,0)
// Ladder Beams
lemlib::Pose LTLB(-13,13); // Ladder: Top Left Beam (LTC-LLC)
lemlib::Pose LTRB(13,13); // Ladder: Top Right Beam (LTC-LRC)
lemlib::Pose LBLB(-13,-13); // Ladder: Bottom Left Beam (LBC-LLC)
lemlib::Pose LBRB(13,-13); // Ladder: Bottom Right Beam (LBC-LRC)
// STARTING POSITIONS
lemlib::Pose autonRedPos(-50,24,270); // -50 24 270
lemlib::Pose autonBluePos(50,24,90); // 50 24 90

void set_drive(double inches, int time, float minSpeed, float maxSpeed) {
    double trueAngle = chassis.getPose(true, true).theta;
    chassis.moveToPoint(cos(trueAngle)*inches + chassis.getPose().x, sin(trueAngle)*inches + chassis.getPose().y, time, {.forwards = inches > 0, .maxSpeed=maxSpeed, .minSpeed=minSpeed});
}


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

void skills() {
    chassis.setPose(-63, 0, 90); // starts at middle of red alliance line
    setIntake(127); // score on alliance stake
    pros::Task lb_task(LBLoop);
    pros::delay(500);

    set_drive(13); // move away from alliance stake
    chassis.waitUntilDone();

    // chassis.moveToPoint(-50, 0, 3000); // move away from alliance stake
    // chassis.waitUntilDone();
    

    chassis.turnToPoint(-50, -18, 3000, {.forwards = false}); // move to mogo
    chassis.waitUntilDone();
    // chassis.moveToPoint(-50, -18, 3000, {.forwards = false}); // move to mogo
    // chassis.waitUntilDone();

    set_drive(-18);
    chassis.waitUntilDone();

    mogoClamp.toggle(); // clamp mogo

    chassis.turnToPoint(-22, -24 + 3, 3000);
    chassis.waitUntilDone();

    chassis.follow(skills1_txt, 15, 3000); // pure pursuit two rings
    chassis.waitUntilDone();
    pros::delay(1000);
    ChangeLBState(PROPPED); // prop up ladybrown
    // chassis.moveToPoint(-22 - 2, -24 + 3, 3000); // pickup ring outside ladder
    // chassis.waitUntilDone();
    // pros::delay(600);

    // chassis.moveToPoint(24, -48, 3000); // move to farther ring
    // chassis.waitUntilDone();
    // chassis.moveToPoint(24, -48, 3000); // move to farther ring
    // chassis.waitUntilDone();

    chassis.turnToPoint(-8, -53, 3000); // turn to go to neutral wall stake
    chassis.waitUntilDone();

    // chassis.moveToPoint(-8, -53, 3000); // pickup ring next to wall stake
    // chassis.waitUntilDone();

    set_drive(13.9); // pickup ring next to wall stake
    chassis.waitUntilDone();

    pros::delay(1000);
    chassis.turnToHeading(180, 3000);
    chassis.waitUntilDone();
    // chassis.moveToPoint(-7 - 1, -60, 3000); // move to wall stake

    set_drive(7); // move to wall stake

    intake.move(0);
    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(1000);
    // chassis.moveToPoint(-8, -50, 3000, {.forwards = false}); // go back a bit

    set_drive(-10 - 2); // go back a bit
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    chassis.turnToPoint(-61, -48, 3000);
    chassis.waitUntilDone();
    // chassis.moveToPoint(-61, -48, 3000); // collect two bottom-left rings

    set_drive(51, 3000, 20, 50); // go slow collecting 3 rings
    chassis.waitUntilDone();
    pros::delay(1000);
    chassis.turnToPoint(-47, -58, 3000); // turn to last bottom-left ring
    chassis.waitUntilDone();
    // chassis.moveToPoint(-47, -58, 3000); // collect last bottom-left ring
    
    set_drive(17.2); // collect last bottom-left ring
    chassis.waitUntilDone();
    pros::delay(600);
    chassis.turnToPoint(-57.2, -57.2, 1000, {.forwards = false});
    chassis.waitUntilDone();
    //chassis.moveToPoint(-57.2, -57.2, 500, {.forwards = false}); // move to corner

    set_drive(-10); // move to corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-60,-60, 500, {.forwards=false}); // look to center
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo

    //chassis.moveToPoint(-48+2, -48, 3000); // move out of corner

    set_drive(14.49); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-48,22,3000, {.forwards=false});
    chassis.waitUntilDone();
    setIntake(0);
    //chassis.moveToPoint(-48,22 + 4, 3000, {.forwards=false}); // move to mogo

    set_drive(-74); // move to mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    pros::delay(600);
    chassis.turnToPoint(-20, 24 + 2, 3000);
    chassis.waitUntilDone();
    setIntake(127);
    //chassis.moveToPoint(-20, 26, 3000); // move to ring outside ladder

    set_drive(24); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(700);
    //chassis.moveToPoint(-24, 24, 3000, {.forwards = false}); // move back

    set_drive(-4.47); // move back
    chassis.waitUntilDone();
    chassis.turnToPoint(-3, 55, 3000);
    chassis.waitUntilDone();
    //chassis.moveToPoint(0, 61, 3000); // move to ring next to wall stake

    set_drive(44.1); // move to ring next to wall stake
    chassis.waitUntilDone();
    pros::delay(1200);
    chassis.turnToPoint(-26, 46 + 3, 3000);
    chassis.waitUntilDone();
    //chassis.moveToPoint(-26, 50 - 1, 3000); // move to ring outside ladder

    set_drive(28.6); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(700);
    //chassis.moveToPoint(-24, 49, 3000, {.forwards=false});

    set_drive(-2); // move back
    chassis.waitUntilDone();
    chassis.turnToPoint(-60 + 3, 50 + 2, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(-57, 52, 3000); // move to two top-left rings

    set_drive(33.13); // move to two top-left rings
    chassis.waitUntilDone();
    pros::delay(1200);
    chassis.turnToPoint(-48 + 3, 63 - 3, 1500);
    chassis.waitUntilDone();
    //chassis.moveToPoint(-45, 63 - 3, 3000); // move to top ring

    set_drive(14.4); // move to top ring
    pros::delay(600);
    chassis.waitUntilDone();
    chassis.turnToPoint(-57.2, 57.2, 3000, {.forwards=false});
    chassis.waitUntilDone();
    //chassis.moveToPoint(-57.2, 57.2, 3000, {.forwards=false}); // back turn to corner

    set_drive(-12.36); // back turn to corner
    chassis.waitUntilDone();
    chassis.turnToPoint(-59, 60, 500, {.forwards=false});
    chassis.waitUntilDone();
    //chassis.moveToPoint(-59, 60, 500, {.forwards=false}); // move to corner

    set_drive(-3.32); // move to corner
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    chassis.waitUntilDone();
    pros::delay(200);
    chassis.turnToPoint(-48, 48, 3000);
    chassis.waitUntilDone();
    //chassis.moveToPoint(-48, 48, 3000); // back out of corner

    set_drive(16.27); // back out of corner
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



    chassis.moveToPoint(24, -24, 3000, {.maxSpeed=100}); // move to ring
    chassis.waitUntilDone();
    chassis.turnToPoint(60 - 5, 0, 3000);
    chassis.waitUntilDone();
    chassis.moveToPoint(60 - 5, 0, 3000);
    chassis.waitUntilDone();
    chassis.turnToPoint(72, -72, 3000, {}); // turn to corner
    chassis.waitUntilDone();
    chassis.moveToPoint(72, -72, 3000, {.minSpeed=127}); // move to corner
    chassis.waitUntilDone();
    pros::delay(1000);
    chassis.moveToPoint(72 - 20, -72 + 20, 3000, {.forwards = false});
    chassis.waitUntilDone();
}


// Auton Development based on Barcbots (11101B)
// Preload one ring
void ringAutonBlue() {
  chassis.setPose(autonBluePos.x, autonBluePos.y, 90); // Set position for left auton
  //mogoClamp.toggle();
  //chassis.moveToPoint((URM.x + 4 - 1), URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Move to upper left mobile goal
  set_drive(23); // Move to upper left mobile goal
  chassis.waitUntil(17 + 2);
  // Mogo piston activate
  chassis.waitUntilDone();
  mogoClamp.toggle();
  //pros::Task color_task(colorSortLoop); // Start color sorting task
  //pros::delay(200);
  setIntake(127); // turn on intake
  pros::delay(400);
  // Turn to and move to middle stacks of rings
  chassis.turnToPoint((TSBRR.x), (TSBRR.y - 5), 2000,{},false); // turn to rings
  chassis.follow(ringRushBlueOld_txt, 15, 3500); // pure pursuit move while intaking rings
  chassis.waitUntil(23);
  chassis.cancelMotion();
  pros::delay(1500); // wait a little
  chassis.follow(ringRushBlueOld_txt, 15, 3500); // pure pursuit move while intaking rings
  chassis.waitUntilDone();

  chassis.moveToPoint(15, 24, 2000, {.forwards=false}); // move back a lot
  chassis.waitUntilDone();
  chassis.turnToPoint(18 + 3, 48 - 3.5, 3000); // turn to 3rd two stack
  chassis.waitUntilDone();
  intakeLift.toggle();
  set_drive(21.36); // move to 3rd two stack
  //chassis.moveToPoint((18 + 3) * sgn, 48 - 3.5, 3000); // move to 3rd two stack
  chassis.waitUntilDone();
  pros::delay(550);
  set_drive(-3.5); // move back a little
  //chassis.moveToPoint((18) * sgn, 48 - 8.5 + 1.5, 1000); // move back a little
  chassis.waitUntilDone();
  //mogoClamp.toggle(); // drop mogo

}

void ringAutonRed() {
  chassis.setPose(-autonBluePos.x, autonBluePos.y, -90); // Mirror position for right auton
  //chassis.moveToPoint(-(URM.x + 4 - 1), URM.y, 3000, {.forwards=false, .minSpeed = 90}); // Mirror move to upper right mobile goal
  set_drive(23); // Move to upper left mobile goal
  chassis.waitUntilDone();
  mogoClamp.toggle(); // clamp mogo
  setIntake(127);
  pros::delay(400);
  chassis.turnToPoint(-TSBRR.x, (TSBRR.y - 5), 2000,{},false); // turn to middle rings
  chassis.follow(ringRushRedOld_txt, 15, 3500); // Use mirrored path file
  chassis.waitUntil(23);
  chassis.cancelMotion(); // stop to intake rings
  pros::delay(1500);
  chassis.follow(ringRushRedOld_txt, 15, 3500); // continue with path
  chassis.waitUntilDone();

  chassis.moveToPoint(-15, 24, 2000, {.forwards=false}); // move back a lot
  chassis.waitUntilDone();
  chassis.turnToPoint(-(18 + 3), 48 - 3.5, 3000); // turn to third two stack
  chassis.waitUntilDone();
  intakeLift.toggle(); // lift inake
  set_drive(21.36); // move and intake third two stack
  chassis.waitUntilDone();
  pros::delay(550);
  set_drive(-3.5); // move back a little
  chassis.waitUntilDone();
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

void mogoSoloAWPBlue() {
    lemlib::Pose start(60,-27.5+(14.5/2),-90);
    intakeLift.toggle();
    chassis.setPose(start.x, start.y, -90);
    intake.move(127);
    
    chassis.turnToPoint(54, 0, 3000, {.minSpeed = 60, .earlyExitRange = 3});
    chassis.waitUntilDone();
    
    set_drive(14.32, 2000, 60); // sqrt((54-60)^2 + (0-(-13))^2)
    chassis.waitUntilDone();
    
    intakeLift.toggle();
    pros::Task colorUntil(intakeUntilColor);
    
    set_drive(-14.32, 1500, 50); // -sqrt((60-54)^2 + (-13-0)^2)
    chassis.waitUntilDone();
    
    chassis.turnToPoint(58, -6.5, 3000, {.minSpeed = 50});
    chassis.waitUntilDone();
    
    set_drive(6.8, 3000); // sqrt((58-60)^2 + (-6.5-(-13))^2)
    chassis.waitUntilDone();
    
    intake.move(0);
    chassis.turnToHeading(-93, 3000, {.maxSpeed = 50});
    chassis.waitUntilDone();
    
    stopColorUntilFunction();
    
    set_drive(-8.56, 1500, 0, 60); // -sqrt((66.5-58)^2 + (-7.5-(-6.5))^2)
    chassis.waitUntilDone();
    
    // Continue converting remaining movements following the same pattern...
}

void mogoSoloAWPRed() {
    lemlib::Pose start(-60,-27.5+(14.5/2),90);
    intakeLift.toggle();
    chassis.setPose(start.x, start.y, 90);
    intake.move(127);
    
    chassis.turnToPoint(-54, 0, 3000, {.minSpeed = 60, .earlyExitRange = 3});
    chassis.waitUntilDone();
    
    set_drive(14.32, 2000, 60); // sqrt((-54-(-60))^2 + (0-(-13))^2)
    chassis.waitUntilDone();
    
    intakeLift.toggle();
    pros::Task colorUntil(intakeUntilColor);
    
    set_drive(-14.32, 1500, 50); // -sqrt((-60-(-54))^2 + (-13-0)^2)
    chassis.waitUntilDone();
    
    chassis.turnToPoint(-58, -6.5, 3000, {.minSpeed = 50});
    chassis.waitUntilDone();
    
    set_drive(6.8, 3000); // sqrt((-58-(-60))^2 + (-6.5-(-13))^2)
    chassis.waitUntilDone();
    
    intake.move(0);
    chassis.turnToHeading(93, 3000, {.maxSpeed = 50});
    chassis.waitUntilDone();
    
    stopColorUntilFunction();
    
    set_drive(-8.56, 1500, 0, 60); // -sqrt((-66.5-(-58))^2 + (-7.5-(-6.5))^2)
    chassis.waitUntilDone();
    
    // Continue converting remaining movements following the same pattern...
}

// Auto-generated code from DSL

void mogoRushAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
	chassis.setPose(55*sgn,-60,120 * sgn); //Starting Line
	set_drive(-50, 2500, 90, 127); //Move next to ring stack
	chassis.waitUntil(48);
  chassis.cancelMotion();
  pros::delay(4000);
  //chassis.swingToHeading(90 * sgn, isBlue?lemlib::DriveSide::RIGHT:lemlib::DriveSide::LEFT, 3000); // swing into mogo
  //chassis.waitUntilDone();
  mogoClamp.toggle();
  pros::delay(100);
	setIntake(127);
  pros::delay(400);

  set_drive(20); // move from middle
  chassis.waitUntilDone();
	mogoClamp.toggle(); //unclamp mogo#1
	chassis.waitUntilDone();
  chassis.turnToPoint(24 * sgn, -24, 3000, {.forwards=false}); //Turn to mogo #2
  chassis.waitUntilDone();
  set_drive(-24); //Move to mogo #2
	chassis.waitUntilDone();
  set_drive(10); //Move back a little
	chassis.waitUntilDone();

  mogoClamp.toggle(); //Clamp mogo #2
	chassis.turnToPoint(0*sgn,-35,3000);//Turn to pushed ring
	chassis.waitUntilDone();
  
  set_drive(24);//Move to pushed ring
	chassis.waitUntilDone();
  set_drive(-48, 3000, 60, 127);//Move backwards
	chassis.waitUntilDone();
  pros::delay(200);
  setIntake(-127);
  chassis.turnToPoint(48 * sgn, -65, 3000); // turn to red ring
  chassis.waitUntilDone();
  set_drive(36); // clear away red ring
  chassis.waitUntilDone();
  set_drive(-12); // move back
  chassis.waitUntilDone();
  setIntake(127);
  chassis.turnToPoint(72 * sgn, -72, 3000); // turn to corner
  chassis.waitUntilDone();
  set_drive(32); // move to corner
  chassis.waitUntilDone();
  pros::delay(300);
  set_drive(-48, 2000, 60, 127); // move back
  chassis.waitUntilDone();
  chassis.turnToPoint(0 * sgn, -20, 3000); // turn to ladder
  chassis.waitUntilDone();
  set_drive(24); // move to ladder
  chassis.waitUntilDone();
}

void disruptRingRush(bool isBlue) {
  int sgn=isBlue?1:-1;
	chassis.setPose(55 * sgn,30,-(71) * sgn); //Starting Line 71
  setIntake(0);
  set_drive(39 - 1, 2500, 100, 127); //move and grab to rings 
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  set_drive(-12 + 3, 2000, 70, 127); // move back a little
  chassis.waitUntilDone();
  chassis.turnToHeading(-97 * sgn, 500); // turn to go back
  chassis.waitUntilDone();
  set_drive(-20 - 6, 1500, 70, 127); // move back
  chassis.waitUntil(6 - 1.5);
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  // chassis.turnToPoint(24 * sgn, 21, 3000, {.forwards = false}); // turn to mogo
  //chassis.swingToHeading(55 * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT, 3000);
  chassis.turnToHeading(60 * sgn, 3000); // turn to mogo
  chassis.waitUntilDone();
  set_drive(-38 + 0.5, 2000); // move to mogo
  chassis.waitUntil(27.5 + 4);
  mogoClamp.toggle(); // clamp mogo
  chassis.waitUntilDone();
  pros::delay(500);
  intake.move(127); // start intake
  chassis.turnToHeading(0 * sgn, 3000); // turn to intake rings
  //chassis.turnToPoint((24 - 4) * sgn, 60, 3000); // turn to intake rings
  chassis.waitUntilDone();
  pros::delay(100);
  set_drive(11, 2000, 80); // move to intake rings
  chassis.waitUntilDone();
  pros::delay(400);
  set_drive(11, 2000, 80);
  chassis.waitUntilDone();
  pros::delay(400);
  chassis.turnToHeading((-50) * sgn, 3000); // turn to last ring
  chassis.waitUntilDone();
  pros::delay(100);
  set_drive(7.5 + 1.5, 1500); // move to last ring
  chassis.waitUntilDone();
  pros::delay(500);
  set_drive(-24-0.5, 2000, 70, 127); // move back
  chassis.waitUntilDone();

  // chassis.turnToHeading(162 * sgn, 1000, {.earlyExitRange = 4}); // turn to two stack middle
  // chassis.waitUntilDone();
  // set_drive(20 - 4, 2500, 75, 127); // move to two stack middle
  // intakeLift.toggle();
  // chassis.waitUntilDone();
  // set_drive(12 - 2, 1000, 0, 30); // keep forward
  // chassis.waitUntilDone();
  // pros::delay(100);
  // set_drive(-5, 1500, 60, 127); // move back
  // intakeLift.toggle();
  // chassis.waitUntilDone();

  // chassis.turnToPoint(24, 0, 3000); // turn to ladder
  // chassis.waitUntilDone();
  // chassis.moveToPoint(24, 0, 3000); // turn to ladder
  // chassis.waitUntilDone();
  
}




void SigSoloAWP(bool isBlue) {
  int sgn=isBlue?1:-1;
  int offset = 0;
  if (isBlue) {
    	chassis.setPose(57.25*sgn,16, 180 * sgn); //Starting Line
  } else {
      offset = 2;
    	chassis.setPose((57.25 - 3)*sgn,(16 + 5), 180 * sgn); //Starting Line
  }

      // Put Preload on AWS

  ChangeLBState(PROPPED); // Ready Lady Brown to intake ring into it 
  set_drive(8 + offset / 2, 2000, 50, 127); // move to AWS
  chassis.waitUntilDone();

// Prepares Preload to be scored
  intake.move(122); // Intake Ring to raised Ladybrown

// Turning Code
  // chassis.turnToPoint(72 * sgn, 0 - 0.5, 750, {.minSpeed = 60}); // turn to AWS
  chassis.turnToHeading((115 - offset * 3) * sgn, 750);
  chassis.waitUntilDone();

// Ring is fully on Ladybrown

// Align to put ring on AWS using Ladybrown
  set_drive(-4.5 + offset / 1.5, 1000, 10, 80); // move back
  chassis.waitUntilDone();
  intake.move(0); // Stop Intaking

//Put ring on AWS
  ChangeLBState(FULLEXTENDED); // Extend Ladybrown
  pros::delay(650); // Delay so Ring can actually stay on the AWS

  ChangeLBState(REST); // retract ladybrown
  set_drive(2, 1000, 50, 80); // move back
  chassis.waitUntilDone();

    // Finished With AWS Preload

  chassis.turnToPoint((21) * sgn, 30 - 1, 500, {.forwards = false}); // quick turn to mogo
  chassis.waitUntilDone();
  // set_drive(-35 - 4); // move to mogo

    // Mogo Clamping

// Automatically Turns to Mogo  
  chassis.moveToPoint(21 * sgn, 29 - offset, 3000, {.forwards = false, .minSpeed = 80}); // move to mogo
  chassis.waitUntil(37 - offset * 2); // Wait at Mogo for clamp consistantly
  mogoClamp.toggle(); // clamp mogo
  chassis.cancelMotion();
  chassis.moveToPoint((13+offset) * sgn, 35 - offset, 3000, {.forwards = false, .minSpeed = 80}); // Move a bit past the mogo

  chassis.waitUntilDone();
  ChangeLBState(EXTENDED); // just to change it
  ChangeLBState(REST); // retract ladybrown
  //pros::delay(20000);

    // Ring Rush Part

// Intaking middle Rings
  chassis.turnToPoint((-10) * sgn, (TSBRR.y + 6 + offset * 4), 2000, {}, false); // turn to rings
  intake.move(127); // Move forwards
// Turning while moving, needs to be tuned
  //chassis.follow(ringRushBlue_txt, 20, 3500); // pure pursuit move while intaking rings
  set_drive(15 - 3.5, 2000, 70);
  chassis.waitUntilDone();
// Wait for the last move to be finished and stop
  // chassis.waitUntil(22 - 8);
  // chassis.cancelMotion();

// Get in position to get the last ring
  chassis.turnToHeading(-3.5 * sgn, 1500); // Turn around
  chassis.waitUntilDone();
  //chassis.follow(ringRushBlue_txt, 20, 1250); // pure pursuit move while intaking rings
  set_drive(18 - offset, 1500, 0, 80);
  chassis.waitUntilDone();
  pros::delay(1000);

// Getting the last non-corner ring on this side
  chassis.moveToPoint(13 * sgn, 35 - 8 + offset * 4, 2000, {.forwards=false, .minSpeed = 80}); // move back a lot
  chassis.waitUntilDone();
  //chassis.turnToPoint(19 - 2, 48, 1500, {.minSpeed = 60}); // turn to 3rd two stack
  chassis.turnToHeading(30 * sgn, 1000);
  chassis.waitUntilDone();

// Still intaking for last ring for 3rd ring on Mogo
  set_drive(23 - 4, 1500, 80); // move to 3rd two stack
  chassis.waitUntilDone();
  // set_drive(-25);
  // chassis.waitUntilDone();

    // Finished with the Ring Rush

    // Left Side, untested and probably not working

    chassis.turnToPoint(24, 0, 3000); // turn to ladder
    chassis.waitUntilDone();
    chassis.moveToPoint(24, 0, 3000); // turn to ladder
    chassis.waitUntilDone();

  // chassis.turnToPoint(45 * sgn, 0 - 3, 2000, {.minSpeed = 60}); // turn to move around ladder
  // chassis.waitUntilDone();
  // set_drive(43 + 4, 2500, 80); // move around ladder
  // chassis.waitUntilDone();
  // set_drive(-5);
  // chassis.waitUntilDone();
  // mogoClamp.toggle(); // unclamp mogo
  // intake.move(-127);
  // set_drive(20, 2000, 60);
  // chassis.waitUntilDone();
  // chassis.turnToPoint((24 - 5) * sgn, -24 + 14, 3000, {.forwards = false}); // turn to mogo
  // chassis.waitUntilDone();
  // set_drive(-30, 2000, 70); // move to mogo
  // chassis.waitUntilDone();
  // mogoClamp.toggle(); // clamp mogo
  // intake.move(127);
  // chassis.turnToPoint(24 * sgn, -60, 3000); // turn to intake rings
  // chassis.waitUntilDone();
  // set_drive(36); // move to intake rings
  // chassis.waitUntilDone();
  // set_drive(-48); // move back
  // // Going to Ladder
  // chassis.turnToPoint(0, -24, 3000);
  // chassis.waitUntilDone();
  // set_drive(24); // move to ladder
  // chassis.waitUntilDone();
  
}





























void mogoAdvayAutonBlue() {
    // Set initial position
    chassis.setPose(48, 60, -90);
    set_drive(-24.5, 3000, 0, 127);
    chassis.waitUntilDone();
    // Turn to Mogo #1
    chassis.turnToPoint(9.5, -55, 3000);
    chassis.waitUntilDone();
    set_drive(-14.87, 3000, 0, 127);
    // -1*sqrt((23.5-9.5)^2+(60-55)^2)
    chassis.waitUntilDone();
    // Clamp to Mogo #1
    mogoClamp.toggle();
    pros::delay(100);
    setIntake(127);
    pros::delay(500);
    // Turn to Mogo #2
    chassis.turnToPoint(17, 19, 3000);
    mogoClamp.toggle();
    chassis.waitUntilDone();
    set_drive(36.77, 3000, 0, 127);
    // -1*sqrt((9.5-17)^2+(55-19)^2)
    chassis.waitUntilDone();
    // Clamp to Mogo #2
    mogoClamp.toggle();
    pros::delay(700);
    // Unknown command: setIntake;
    // Turn to Ring Stake
    chassis.turnToPoint(25, -54, 3000);
    chassis.waitUntilDone();
    set_drive(12, 3000, 0, 127);
    chassis.waitUntilDone();
    // Turn to Ladder
    chassis.turnToPoint(48, 0, 3000);
    chassis.waitUntilDone();
    set_drive(40, 3000, 0, 127);
}

void mogoAdvayAutonRed() {
    // Set initial position
    chassis.setPose(-48, 60, 90);
    set_drive(-24.5, 3000, 0, 127);
    chassis.waitUntilDone();
    // Turn to Mogo #1
    chassis.turnToPoint(-9.5, -55, 3000);
    chassis.waitUntilDone();
    set_drive(-14.87, 3000, 0, 127);
    // -1*sqrt((23.5-9.5)^2+(60-55)^2)
    chassis.waitUntilDone();
    // Clamp to Mogo #1
    mogoClamp.toggle();
    pros::delay(100);
    setIntake(127);
    pros::delay(500);
    // Turn to Mogo #2
    chassis.turnToPoint(-17, 19, 3000);
    mogoClamp.toggle();
    chassis.waitUntilDone();
    set_drive(36.77, 3000, 0, 127);
    // -1*sqrt((9.5-17)^2+(55-19)^2)
    chassis.waitUntilDone();
    // Clamp to Mogo #2
    mogoClamp.toggle();
    pros::lcd::set_text(3, std::to_string(optical.get_proximity()));
    pros::delay(700);
    // Unknown command: setIntake;
    // Turn to Ring Stake
    chassis.turnToPoint(-25, -54, 3000);
    chassis.waitUntilDone();
    set_drive(12, 3000, 0, 127);
    chassis.waitUntilDone();
    // Turn to Ladder
    chassis.turnToPoint(-48, 0, 3000);
    chassis.waitUntilDone();
    set_drive(40, 3000, 0, 127);
}

/*void mogoAdvayAuton(bool isBlue) {
  int sgn=isBlue?1:-1;
  //mogoClamp.toggle();
	chassis.setPose(48*sgn,-60,90 * sgn); //Starting Line
	chassis.moveToPoint((21 + 2.5)*sgn,-60,3000,{.forwards=false, .minSpeed = 90}); //Move next to ring stack
	chassis.waitUntil(25);
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

}*/
