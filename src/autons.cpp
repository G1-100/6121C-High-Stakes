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

void set_drive_advanced(double inches, int time, float minSpeed, float maxSpeed, float earlyExitRange) {
    double trueAngle = chassis.getPose(true, true).theta;
    chassis.moveToPoint(cos(trueAngle)*inches + chassis.getPose().x, sin(trueAngle)*inches + chassis.getPose().y, time, {.forwards = inches > 0, .maxSpeed=maxSpeed, .minSpeed=minSpeed, .earlyExitRange = earlyExitRange});
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

void simpleRing(bool isBlue) {
	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, 90 * sgn);
	set_drive(-34 - 2, 2500, 0, 65);
	chassis.waitUntil(28);
	mogoClamp.toggle();
	chassis.waitUntilDone();
	//pros::delay(1000);
	pros::delay(500);
	intake.move_voltage(12000);
	pros::delay(700);
	chassis.turnToHeading(0, 2000);
	chassis.waitUntilDone();
	set_drive(21, 2000);
	chassis.waitUntilDone();
	pros::delay(1500);
	set_drive(-10, 1500);
	chassis.waitUntilDone();
	chassis.turnToHeading(180, 2000);
	chassis.waitUntilDone();
	set_drive(30, 2500, 0, 70);
	chassis.waitUntilDone();
  pros::delay(1000);
  intake.move(0);
	
}

void verySimpleMogo(bool isBlue) {
	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, 90 * sgn);
	set_drive(-34 - 2, 2500, 0, 65);
	chassis.waitUntil(28);
	mogoClamp.toggle();
	chassis.waitUntilDone();
	//pros::delay(1000);
	pros::delay(500);
	intake.move_voltage(12000);
	pros::delay(700);
	chassis.turnToHeading(180, 2000);
	chassis.waitUntilDone();
	set_drive(21, 2000);
	chassis.waitUntilDone();
	pros::delay(1500);
	set_drive(-10, 1500);
	chassis.waitUntilDone();
	chassis.turnToHeading(0, 2000);
	chassis.waitUntilDone();
	set_drive(30, 2500, 0, 70);
	chassis.waitUntilDone();
  pros::delay(1000);
  intake.move(0);
	
}

void simpleMogo(bool isBlue) {
	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, (33 + 1) * sgn);
  LBState = PROPPED;
  LBRotation.set_position(4600);
  ChangeLBState(FULLEXTENDED);
  pros::delay(650);
  set_drive(-15, 2000);
  chassis.waitUntilDone();
  ChangeLBState(REST);
  chassis.turnToHeading(90 * sgn, 2000);
  chassis.waitUntilDone();
  set_drive(-23 - 11 + 3, 2000, 0, 70);
  chassis.waitUntil(25 + 1);
  mogoClamp.toggle();
  chassis.waitUntilDone();

	// set_drive(-34, 2000);
	// chassis.waitUntil(32 - 4);
	// mogoClamp.toggle();
	// chassis.waitUntilDone();
	intake.move_voltage(12000);
	//pros::delay(1000);
	chassis.turnToHeading(180, 2000);
	chassis.waitUntilDone();
	set_drive(21, 2000);
	chassis.waitUntilDone();
	pros::delay(500);
	set_drive(-20, 1500);
	chassis.waitUntilDone();
	//chassis.turnToHeading(0, 2000);
  chassis.turnToHeading((45 - 2) * sgn, 1500, {.maxSpeed = 80});
	chassis.waitUntilDone();
	set_drive(50 - 20, 2000, 0, 60);
	chassis.waitUntilDone();
  intake.move(127);
  pros::delay(1000);
  set_drive(15, 1500);
  chassis.waitUntilDone();
  set_drive(-24 - 15, 1500, 70, 120);
  chassis.waitUntilDone();
  chassis.turnToHeading(-45 * sgn, 2000);
  chassis.waitUntilDone();
  // set_drive(5 + 1, 2000, 70, 120);
  // chassis.waitUntilDone();
  ChangeLBState(EXTENDED);
	
}

void newMogoRush(bool isBlue) {
  int sgn=isBlue?1:-1;
  chassis.setPose(0, 0, -110 * sgn); // Set position
  LBState = PROPPED;
  LBRotation.set_position(4600);
  set_drive(37 + 2, 2500, 126, 127); // Move to first mogo
  chassis.waitUntil(11 + 1);
  ChangeLBState(ALMOSTFULLEXTENDED);
  chassis.waitUntilDone();
  // pros::delay(500);
  // chassis.turnToHeading(-150 * sgn, 2000, {.minSpeed = 60}); // Turn mogo to disrupt
  // chassis.waitUntilDone();
  set_drive(-19.5 - 25 + 12, 1500, 120); // Move back
  chassis.waitUntil(30 - 14);
  ChangeLBState(FULLEXTENDED);
  chassis.waitUntilDone();
  ChangeLBState(REST);
  chassis.turnToHeading((-180 - 60 -10+3) * sgn, 2000); // Turn to second mogo
  pros::delay(100);
  callLBReset();
  chassis.waitUntilDone();
  set_drive(-36.5 + 15, 2000); // Move to second mogo
  chassis.waitUntil(33 - 15);
  mogoClamp.toggle(); // Clamp second mogo
  chassis.waitUntilDone();
  set_drive(20, 2000, 65, 127); // Move back
  chassis.waitUntilDone();
  chassis.turnToHeading((-140 + 5 + 5 + 5) * sgn, 2000, {.maxSpeed = 90, .minSpeed = 30, .earlyExitRange = 1}); // Turn to two stack
  chassis.waitUntilDone();
  intake.move(127); // Turn on intake
  set_drive(34.5 - 3 + 4, 2000); // Move to two stack
  chassis.waitUntilDone();
  pros::delay(200);
  chassis.turnToHeading(isBlue?(106-10 + 5):-106, 2000); // turn to corner
  chassis.waitUntilDone();
  doinker.toggle();
  set_drive(40 - 5, 1500, 60, 127); // Move to corner
  chassis.waitUntilDone();
  pros::delay(500);
  chassis.turnToHeading(isBlue?-180:310, 1500, {.minSpeed = 80, .earlyExitRange = 1.5}); // Turn to knock away corner rings
  chassis.waitUntilDone();
  doinker.toggle();
  chassis.turnToHeading(-260 * sgn, 2000, {.minSpeed = 50, .earlyExitRange = 1.5}); // Turn to 2nd mogo
  chassis.waitUntilDone();
  set_drive(15 - 5, 2000, 60, 120); // Move forward
  chassis.waitUntilDone();
  set_drive(-10, 2000); // Move back
  chassis.waitUntilDone();
  // chassis.turnToHeading(-300 * sgn, 1500, {.minSpeed = 60, .earlyExitRange = 1.5}); // Turn to 2nd ring
  // chassis.waitUntilDone();
  // set_drive(17 - 1, 2000); // Move to 2nd ring
  // chassis.waitUntilDone();
  //pros::delay(200);
  //chassis.turnToHeading(300, 2000);
  // set_drive(-15 + 3, 2000); // Move back
  // chassis.waitUntilDone();
  chassis.turnToHeading(-40 * sgn, 2000, {.minSpeed = 40, .earlyExitRange = 1}); // Turn to ladder
  chassis.waitUntilDone();
  set_drive(46, 2000); // Move to ladder
  ChangeLBState(SEMIEXTENDED);
  chassis.waitUntilDone();
  ChangeLBState(EXTENDED);
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
    //pros::Task colorUntil(intakeUntilColor);
    
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
    //pros::Task colorUntil(intakeUntilColor);
    
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

void disruptRingRush(bool isBlue) {
  int sgn=isBlue?1:-1;
	chassis.setPose(55 * sgn,30,-(71 + 2.5) * sgn); //Starting Line 71
  setIntake(0);
  set_drive(38 + 1.5, 2500, 100, 127); //move and grab to rings 
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  set_drive(-12 + 1.5 - 3, 2000); // move back a little
  chassis.waitUntilDone();
  chassis.turnToHeading(-97 * sgn, 500); // turn to go back
  chassis.waitUntilDone();
  set_drive(-4.5 - 3, 1500, 70, 127); // move back
  chassis.waitUntil(1 + 3);
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  // chassis.turnToPoint(24 * sgn, 21, 3000, {.forwards = false}); // turn to mogo
  //chassis.swingToHeading(55 * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT, 3000);
  chassis.turnToHeading((33 - 2) * sgn, 2000, {.maxSpeed = 100}); // turn to mogo
  chassis.waitUntilDone();
  set_drive(-18.5 - 5 + 3, 1500, 0, 65 + 10);
  chassis.waitUntil(14 + 1.5);
  mogoClamp.toggle(); // clamp mogo
  chassis.cancelMotion();
  set_drive(-12 + 1.5, 1500, 70, 120); // move to mogo
  chassis.waitUntilDone();
  chassis.turnToHeading((-8) * sgn, 3000, {.maxSpeed = 100, .minSpeed = 30, .earlyExitRange = 1}); // turn to intake rings
  //chassis.turnToPoint((24 - 4) * sgn, 60, 3000); // turn to intake rings
  chassis.waitUntilDone();
  pros::delay(100);
  set_drive(41 - 15, 2000, 0, 50 - 5); // move to intake rings
  chassis.waitUntil(8);
  intake.move(127); // start intake
  chassis.waitUntilDone();
  pros::delay(500);
  set_drive(13 - 2, 1500);
  chassis.waitUntilDone();
  
  pros::delay(500);
  chassis.turnToHeading((135 - 1.5) * sgn, 2000, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1}); // turn to preload
  chassis.waitUntilDone();
  set_drive(58.5 - 10, 2000, 70, 127); // move to preload
  chassis.waitUntil(40 - 30);
  ChangeLBState(PROPPED);
  chassis.waitUntilDone();
  pros::delay(900-300);
  chassis.turnToHeading((125 - 5.5) * sgn, 2000, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1});
  chassis.waitUntilDone();
  intake.move(0);
  set_drive(11 - 2.25, 1500); // move to preload and score
  chassis.waitUntilDone();
  ChangeLBState(3);
  pros::delay(700 - 100);
  set_drive_advanced(-24.5 + 3 + 8, 2000, 70, 127, 2); // move back
  chassis.waitUntilDone();
  ChangeLBState(REST);
  
}

void disruptRingRushBlue() {
  int sgn= 1;
	chassis.setPose(55 * sgn,30,-(71 + 2.5) * sgn); //Starting Line 71
  setIntake(0);
  set_drive(38 + 0.25, 2500, 100, 127); //move and grab to rings 
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  set_drive(-12 + 1.5 - 3, 2000); // move back a little
  chassis.waitUntilDone();
  chassis.turnToHeading(-97 * sgn, 500); // turn to go back
  chassis.waitUntilDone();
  set_drive(-4.5 - 3, 1500, 70, 127); // move back
  chassis.waitUntil(1 + 3);
  rushLeftPiston.toggle();
  rushRightPiston.toggle();
  chassis.waitUntilDone();
  // chassis.turnToPoint(24 * sgn, 21, 3000, {.forwards = false}); // turn to mogo
  //chassis.swingToHeading(55 * sgn, isBlue?lemlib::DriveSide::LEFT:lemlib::DriveSide::RIGHT, 3000);
  chassis.turnToHeading((33 - 2) * sgn, 2000, {.maxSpeed = 100, .minSpeed = 30, .earlyExitRange = 1}); // turn to mogo
  chassis.waitUntilDone();
  set_drive(-18.5 - 5 + 3, 1500, 0, 65 + 10);
  chassis.waitUntil(14 + 1.5);
  mogoClamp.toggle(); // clamp mogo
  chassis.cancelMotion();
  set_drive(-12 + 1.5, 1500, 70, 120); // move to mogo
  chassis.waitUntilDone();
  chassis.turnToHeading((-8) * sgn, 3000, {.maxSpeed = 100, .minSpeed = 30, .earlyExitRange = 1}); // turn to intake rings
  //chassis.turnToPoint((24 - 4) * sgn, 60, 3000); // turn to intake rings
  chassis.waitUntilDone();
  pros::delay(100);
  set_drive(41 - 15, 2000, 0, 50 - 5); // move to intake rings
  chassis.waitUntil(8);
  intake.move(127); // start intake
  chassis.waitUntilDone();
  pros::delay(500);
  set_drive(13 - 2, 1500);
  chassis.waitUntilDone();
  
  pros::delay(500);
  chassis.turnToHeading((135 - 1.5) * sgn, 2000, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1}); // turn to preload
  chassis.waitUntilDone();
  set_drive(58.5 - 10, 2000, 70, 127); // move to preload
  chassis.waitUntil(40 - 30);
  ChangeLBState(PROPPED);
  chassis.waitUntilDone();
  pros::delay(900-300);
  chassis.turnToHeading((125 - 5.5) * sgn, 2000, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1});
  chassis.waitUntilDone();
  intake.move(0);
  set_drive(11 - 2.25, 1500); // move to preload
  chassis.waitUntilDone();
  ChangeLBState(3);
  pros::delay(700 - 100);
  set_drive_advanced(-24.5 + 3 + 8, 2000, 70, 127, 2); // move back
  chassis.waitUntilDone();
  ChangeLBState(REST);
  chassis.turnToHeading((-110) * sgn, 2000, {.maxSpeed = 80, .minSpeed = 50, .earlyExitRange = 2}); // turn to ladder
  chassis.waitUntilDone();
  set_drive_advanced(26.5, 1500, 90, 100, 2); // move forward
  chassis.waitUntilDone();
  ChangeLBState(PROPPED); 
}


void testAuton() {
  ChangeLBState(PROPPED);
  intake.move(127);
  // set_drive(50, 3000, 120, 127);
  chassis.waitUntilDone();
}






void safeFourRing(bool isBlue) {


    // GETS CORNER
// Note: it does need to be this fast
// Another Note: Blue side general movements should be good, but most are still off and need to be tuned
// Also things are happening with syncing, these notes are for allowing the auton changes to be commited

	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, (33 + 1) * sgn);

// AWS

  LBState = PROPPED; // Prop LB for preload
  LBRotation.set_position(4400);
  ChangeLBState(EXTENDED); // Extend LB for AWS
  pros::delay(650 - 50);
  ladybrown.move(-15); // jank way to make sure LB doesn't get stuck
  
  set_drive(-11 + 3, 2000, 80); // move back from AWS
  chassis.waitUntilDone();
  ChangeLBState(REST); // retract ladybrown
  chassis.turnToHeading(isBlue?-270:90 * sgn, 1000, {.minSpeed = 70}); // turn to mogo
  chassis.waitUntilDone();
  callLBReset();
  set_drive(-29, 2000, 100); // move to mogo
  chassis.waitUntil(20);
  chassis.cancelMotion();
  set_drive(-10 - 2, 1500, 0, 70);
  chassis.waitUntil(6);
  mogoClamp.toggle();
  chassis.waitUntilDone();
	intake.move_voltage(12000);
	chassis.turnToHeading(180, 2000, {.minSpeed = 80}); // turn to two stack rings
	chassis.waitUntilDone();
	set_drive(21 + 1.5, 2000, 100);
	chassis.waitUntilDone();
	//pros::delay(500);
	set_drive(-20 + 7, 1500, 100);
	chassis.waitUntilDone();
  chassis.turnToHeading(isBlue?-325:(45 - 2) * sgn, 1500, {.minSpeed = 70}); // turn to middle two stack
	chassis.waitUntilDone();
	set_drive(35, 2000, 80, 120); // move to middle two stck
  chassis.waitUntil(18);
  chassis.cancelMotion();
  set_drive(15 + 2, 2500, 0, 60);
  chassis.waitUntilDone();
  intake.move(127);
  pros::delay(500 - 200);
  set_drive(15, 1500, 80); // move to intake right color
  chassis.waitUntilDone();
  set_drive(-40, 1500, 90, 120); // move back
  chassis.waitUntilDone();
  doinker.toggle();
  //chassis.turnToHeading(-225 * sgn, 2000, {.minSpeed = 80}); // face corner
  chassis.turnToHeading(isBlue?-225:225, 1500, {.minSpeed = 80, .earlyExitRange = 1.5}); // Turn to knock away corner rings
  chassis.waitUntilDone();
  set_drive(70 + 3, 1000, 100); // move to corner
  chassis.waitUntilDone();
  //chassis.turnToHeading(-300 * sgn, 2000, {.minSpeed = 80, .earlyExitRange = 2}); // sweep corner
  chassis.turnToHeading(isBlue?-23:300, 1500, {.minSpeed = 80, .earlyExitRange = 1.5}); // Turn to knock away corner rings
  chassis.waitUntilDone();
  doinker.toggle();
  chassis.turnToHeading(isBlue?-180:-240 * sgn, 2000, {.minSpeed = 80}); // face corner
  chassis.waitUntilDone();
  // set_drive(-10, 1000, 80); // move back
  // chassis.waitUntilDone();
  // chassis.turnToHeading(260, 2000, {.minSpeed = 80});
  // chassis.waitUntilDone();
  set_drive(10, 1000, 127);
  chassis.waitUntilDone();
  set_drive(-10, 100, 127);
  chassis.waitUntilDone();
  chassis.turnToHeading(-45 * sgn, 2000, {.minSpeed = 80});
  chassis.waitUntilDone();
  set_drive(42, 1000, 127);
  //chassis.waitUntil(25);
  chassis.waitUntilDone();
  ChangeLBState(EXTENDED);
	
}