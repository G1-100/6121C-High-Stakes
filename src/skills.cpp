#include "main.h"
#include "pros/motors.h"
#include "pros/rtos.hpp"
#include "lemlib/api.hpp"

void skills() {
    chassis.setPose(-60.5, -13, (-49)); // starts at middle of red alliance line
    setIntake(127); // score on alliance stake
    pros::Task lb_task(LBLoop);
    LBState = EXTENDED;
    LBRotation.set_position(4600);
    ChangeLBState(FULLEXTENDED);
    intakeUnstuckActivated = false;
    ColorLoopActive = false;

    pros::delay(650);

    set_drive(-17.5 - 2, 1500, 0, 70); // move away from alliance stake
    chassis.waitUntil(16);


    /////////////////////////// FIRST MOGO ///////////////////////////
    /////////////////////////// FIRST MOGO ///////////////////////////
    

    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
    pros::delay(100);

    chassis.turnToHeading(90, 1200, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1}); // turn to two rings
    chassis.waitUntilDone();

    chassis.follow(skills1_txt, 15, 2500); // pure pursuit two rings
    chassis.waitUntil(30);
    callLBReset();
    chassis.waitUntilDone();
    //pros::delay(800);
    //chassis.turnToHeading(-110, 2000, {.maxSpeed = 60}); // turn to wall stake ring
    chassis.turnToPoint(-1.2156, -54.6709, 2000, {.maxSpeed = 60}); // turn to wall stake ring
    chassis.waitUntilDone();
    ChangeLBState(PROPPED); // prop up ladybrown

    //set_drive(28.65 + 0.2); // pickup ring next to wall stake
    chassis.moveToPoint(-1.2156, -54.6709, 1500);
    chassis.waitUntilDone();

    std::cout << lemlib::format_as(chassis.getPose()) << "\n";

    //pros::delay(500);
    chassis.turnToHeading(183, 1500, {.maxSpeed = 90, .minSpeed = 20, .earlyExitRange = 1}); // turn to wall stake
    //chassis.swingToHeading(180, lemlib::DriveSide::LEFT, 1000);
    chassis.waitUntilDone();
    pros::delay(300 - 150);

    set_drive(5, 1200 - 700); // move to wall stake
    chassis.waitUntil(1.5);
    intake.move(0);
    ChangeLBState(EXTENDED); // extend ladybrown
    chassis.waitUntilDone();
    pros::delay(350 + 50);
    chassis.turnToHeading(180, 700); // correct angle
    chassis.waitUntilDone();

    set_drive(-12.8 - 0.5); // go back a bit
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    chassis.turnToHeading(-89.9 + 1, 1500, {.maxSpeed = 75}); // turn to two rings
    setIntake(127);
    chassis.waitUntilDone();
    intake.move_voltage(12000);

    // collecting 3 rings

    set_drive(56, 3000, 0, 60 + 30 - 25 + 15); 
    chassis.waitUntil(30 - 1);
    chassis.cancelMotion();
    set_drive(27 + 1, 3000, 0, 30 + 40); // intake rings slowly
    chassis.waitUntilDone();
    setIntake(127);
    pros::delay(800 - 200);
    chassis.turnToHeading(140, 2000, {.maxSpeed = 75, .minSpeed = 30, .earlyExitRange = 1.5}); // turn to last ring before corner
    chassis.waitUntil(10);
    //startColorUntil(1); // start color until 1 ring
    chassis.waitUntilDone();
    //stopColorUntilFunction();
    setIntake(127);
    set_drive(14.5 - 1.5, 1500, 75, 120); // collect last bottom-left ring
    chassis.waitUntilDone();
    //startColorUntil(1);
    //pros::delay(400);
    chassis.turnToHeading(70 - 5, 2000, {.maxSpeed = 70, .minSpeed = 30, .earlyExitRange = 2}); // turn to corner
    chassis.waitUntilDone();
    //stopColorUntilFunction();
    setIntake(127);

    set_drive(-14 - 1, 750); // move to corner
    chassis.waitUntilDone();
    pros::delay(300 + 500);
    //setIntake(-30);
    mogoClamp.toggle(); // unclamp mogo
    //pros::delay(350);
    setIntake(0);

    set_drive(6.5 + 0.25, 1500, 60, 120); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToHeading(179.5 - 0.1, 1500, {.maxSpeed = 80});
    chassis.waitUntilDone();
    setIntake(0);

    set_drive(-88 + 1, 2500, 80, 120); // move to mogo
    chassis.waitUntil(60);
    chassis.cancelMotion();
    set_drive(-21, 1500, 0, 60);
    chassis.waitUntil(20 - 0.5+1);
    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();


    /////////////////////////// SECOND MOGO ///////////////////////////
    /////////////////////////// SECOND MOGO ///////////////////////////

    set_drive(3, 500, 40); // move back
    chassis.waitUntilDone();
    chassis.turnToHeading(80 - 2, 1250); // turn to pure pursuit two stacks
    chassis.waitUntilDone();
    std::cout << lemlib::format_as(chassis.getPose()) << "\n";
    setIntake(127);
    callLBReset(); // reset ladybrown
    
    chassis.follow(skills2_txt, 15, 4000); // pure pursuit 3 rings
    chassis.waitUntilDone();
    ChangeLBState(PROPPED);
    set_drive(-30, 1500); // move back
    chassis.waitUntilDone();

    //chassis.turnToHeading(-71.75 + 1.5, 2000, {.maxSpeed = 60});
    chassis.turnToPoint(0.699233+1 - 2, 69.8696, 2000, {.maxSpeed = 60});
    chassis.waitUntilDone();
    startColorUntil(1);
    // ChangeLBState(PROPPED); // prop up ladybrown
    //set_drive(27 + 3.5); // move to ring next to wall stake
    chassis.moveToPoint(0.699233+1 - 2, 69.86966, 1500);
    chassis.waitUntilDone();
    std::cout << lemlib::format_as(chassis.getPose()) << "\n";

    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(300);
    chassis.turnToHeading(0, 700); // correct angle
    chassis.waitUntilDone();
    set_drive(-10, 700); // move back
    ChangeLBState(REST); // retract ladybrown
    chassis.waitUntilDone();
    ChangeLBState(PROPPED);
    set_drive(5, 700); // move to wall stake
    chassis.waitUntil(1);
    setIntake(127);
    chassis.waitUntilDone();
    ChangeLBState(EXTENDED);

    set_drive(-15.5 + 2.5, 3000); // move back
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
    setIntake(127);
    chassis.turnToHeading(-93 + 1.5-3.5, 2000, {.maxSpeed = 80, .minSpeed = 30, .earlyExitRange = 1}); // turn to three rings
    chassis.waitUntilDone();
    intake.move_voltage(12000);

    // collecting 3 rings

    set_drive(57, 2000, 0, 60 + 15);
    callLBReset();
    chassis.waitUntil(30 - 1);
    chassis.cancelMotion();
    set_drive(28.25, 2500, 0, 30 + 30); // intake rings slowly
    chassis.waitUntilDone();
    pros::delay(800);
    chassis.turnToHeading(40, 1000, {.maxSpeed = 70, .minSpeed = 30, .earlyExitRange = 1.5}); // turn to last ring before corner
    chassis.waitUntilDone();
    set_drive(10 + 1.5, 1500, 75, 120); // move to ring before corner
    chassis.waitUntilDone();

    chassis.turnToHeading(107 + 5, 1500, {.maxSpeed = 58, .minSpeed = 60, .earlyExitRange = 2}); // turn to corner
    chassis.waitUntilDone();

    set_drive(-14 + 1, 700); // back INto corner
    chassis.waitUntilDone();
    callLBReset();
    pros::delay(400 - 100);
    intake.move(0);
    mogoClamp.toggle(); // unclamp mogo
    //pros::delay(350);
    ColorLoopActive = true;
    std::cout << lemlib::format_as(chassis.getPose()) << "\n";

    chassis.turnToHeading(112 - 0.5, 700 + 100);
    chassis.waitUntilDone();
    ChangeLBState(PROPPED); // prop up ladybrown
    //startColorUntil(1); // start color until 1 ring

    set_drive(85.27 + 6); // go to intake ring
    chassis.waitUntil(60);
    intake.move_voltage(12000);
    chassis.waitUntilDone();
    //intake.move(0);
    chassis.turnToHeading(-45, 1500, {.maxSpeed = 75}); // turn to third mogo
    chassis.waitUntilDone();
    set_drive(-36 + 1.5 - 1, 2000, 0, 75); // go to third mogo
    chassis.waitUntil(33);
    mogoClamp.toggle();
    //stopColorUntilFunction();


    /////////////////////////// THIRD MOGO ///////////////////////////
    /////////////////////////// THIRD MOGO ///////////////////////////

    ColorLoopActive = true;

    chassis.waitUntilDone();
    pros::delay(200);
    //ChangeLBState(EXTENDED); // extend ladybrown a little
    //setIntake(127);
    chassis.turnToHeading(90 - 2, 1500, {.maxSpeed = 75}); // turn to AWS
    chassis.waitUntilDone();
    set_drive(4.5 + 10, 1500 - 500, 0, 70); // move to AWS
    chassis.waitUntilDone();
    set_drive(-8, 1500); // move back
    setIntake(0);
    ChangeLBState(FULLEXTENDED); // extend ladybrown
    pros::delay(700);
    set_drive(-5 + 0.25, 2000, 60, 120); // move back
    setIntake(127);
    chassis.turnToHeading(-135 - 5, 1500, {.maxSpeed = 90, .minSpeed = 50, .earlyExitRange = 2}); // turn to ring outside ladder
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
    set_drive(32 - 1.5, 2000, 70, 120); // move to ring outside ladder
    chassis.waitUntilDone();
    pros::delay(400);
    chassis.turnToHeading(-45, 1500); // turn to center ring inside ladder
    chassis.waitUntilDone();
    setIntake(100);
    set_drive(30 + 3, 2000); // move to center ring inside ladder
    chassis.waitUntilDone();
    startColorUntil(1); // start color until 1 ring
    chassis.turnToHeading(-35 + 7, 1500); // turn to go back
    chassis.waitUntilDone();
    set_drive(-60 - 3.5, 2000, 50, 127); // move back
    chassis.waitUntilDone();
    chassis.turnToHeading(75 - 20, 1500);
    chassis.waitUntilDone();
    stopColorUntilFunction();
    setIntake(127);
    //chassis.follow(skills3_txt, 15, 2300); // pure pursuit two two stacks


     set_drive(16 - 1, 1500, 100, 120); // move to first stack
    chassis.waitUntilDone();
    //pros::delay(500);
    chassis.turnToHeading(90, 1000);
    chassis.waitUntilDone();
    set_drive(10, 1500, 60, 120); // intake second stack
    chassis.waitUntilDone();
    //pros::delay(500);
    chassis.turnToHeading(-145 - 5, 1500); // turn to third two stack
    chassis.waitUntilDone();
    set_drive(13, 2000); // intake third two stack
    chassis.waitUntilDone();
    pros::delay(500);
    // set_drive(-13, 2000); // move back
    // chassis.waitUntilDone();

    
    chassis.turnToHeading(-58 + 5, 1000, {.minSpeed = 60, .earlyExitRange = 2}); // turn to corner
    //chassis.swingToHeading(-55 + 20, lemlib::DriveSide::LEFT, 1000); // turn to corner
    chassis.waitUntilDone();
    //pros::delay(400);
    mogoClamp.toggle(); // unclamp mogo
    set_drive(-6 - 5, 1200, 100, 120); // move to corner
    chassis.waitUntilDone();
    setIntake(0);
    setIntake(0);
    //pros::delay(200);
    chassis.turnToHeading(-45, 1500, {.minSpeed = 70}); // turn to corner
    chassis.waitUntilDone();
    set_drive(20 - 3, 2000, 90, 127); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToHeading(20, 1500, {.minSpeed = 80, .earlyExitRange = 2}); // turn to 4th mogo


    /////////////////////////// FOURTH MOGO ///////////////////////////
    /////////////////////////// FOURTH MOGO ///////////////////////////


    chassis.waitUntilDone();
    callLBReset();
    std::cout << lemlib::format_as(chassis.getPose()) << "\n";
    set_drive(108 - 4, 3200, 120, 127); // push other mogo to corner
    chassis.waitUntilDone();
    set_drive(-10, 1500, 0, 60); // move back
    // chassis.follow(skills4_txt, 15, 3500); // pure pursuit two two stacks
    // chassis.waitUntilDone();
    // set_drive(-15, 1500, 60, 127); // move back
    // chassis.waitUntilDone();
    // ChangeLBState(PROPPED); // prop up ladybrown
    // chassis.turnToHeading(-50, 1000, {.minSpeed = 60, .earlyExitRange = 2}); // turn to two stack
    // chassis.waitUntilDone();
    // setIntake(127);
    // set_drive(12, 1500, 60, 127); // move back
    // chassis.waitUntilDone();
    // chassis.turnToHeading(0, 1000, {.minSpeed = 60, .earlyExitRange = 2}); // turn to two stack
    // chassis.waitUntilDone();
    // set_drive(10, 1500, 60, 127); // move to two stack
    // chassis.waitUntilDone();
    // set_drive(-80, 3000, 100, 127); // go to AWS
    // chassis.waitUntilDone();

}

void skillsMacro() {
    chassis.setPose(-60.5, -13, (-49)); // starts at middle of red alliance line
    setIntake(127); // score on alliance stake
    pros::Task lb_task(LBLoop);
    LBState = EXTENDED;
    LBRotation.set_position(4600);
    ChangeLBState(FULLEXTENDED);

    pros::delay(650);

    set_drive(-16 - 1.5, 1500); // move away from alliance stake
    chassis.waitUntil(12);
    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
    master.rumble("."); // short rumble to notify driver
}