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

    pros::delay(750);

    set_drive(-16 - 1.5); // move away from alliance stake
    chassis.waitUntil(11);


    /////////////////////////// FIRST MOGO ///////////////////////////
    /////////////////////////// FIRST MOGO ///////////////////////////
    

    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    chassis.turnToHeading(90, 1500); // turn to two rings
    chassis.waitUntilDone();

    chassis.follow(skills1_txt, 15, 3000); // pure pursuit two rings
    chassis.waitUntil(30);
    callLBReset();
    chassis.waitUntilDone();
    pros::delay(800);
    chassis.turnToHeading(-115 + 1, 2000); // turn to wall stake ring
    chassis.waitUntilDone();
    ChangeLBState(PROPPED); // prop up ladybrown

    set_drive(27.5); // pickup ring next to wall stake
    chassis.waitUntilDone();

    pros::delay(900);
    chassis.turnToHeading(180, 3000); // turn to wall stake
    chassis.waitUntilDone();

    set_drive(5, 1000); // move to wall stake
    chassis.waitUntilDone();

    intake.move(0);
    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(600 - 100);

    set_drive(-13.5 - 0.5); // go back a bit
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    chassis.turnToHeading(-90, 1500);
    setIntake(127);
    chassis.waitUntilDone();

    set_drive(57.5, 3000); // collecting 3 rings
    chassis.waitUntil(30);
    chassis.cancelMotion();
    set_drive(27.5, 2000, 0, 70); // intake rings slowly
    chassis.waitUntilDone();
    pros::delay(1300);
    chassis.turnToHeading(138 + 2, 2000);
    chassis.waitUntilDone();
    
    set_drive(16); // collect last bottom-left ring
    chassis.waitUntilDone();
    pros::delay(600);
    chassis.turnToHeading(70, 2000);
    chassis.waitUntilDone();

    set_drive(-14); // move to corner
    chassis.waitUntilDone();
    setIntake(-30);
    mogoClamp.toggle(); // unclamp mogo
    pros::delay(350);
    setIntake(0);


    set_drive(8.5 - 2); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToHeading(179, 1500);
    chassis.waitUntilDone();
    setIntake(0);

    set_drive(-88 + 1); // move to mogo
    chassis.waitUntil(60);
    chassis.cancelMotion();
    set_drive(-25, 1500, 0, 80);
    chassis.waitUntil(21 + 3);
    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();


    /////////////////////////// SECOND MOGO ///////////////////////////
    /////////////////////////// SECOND MOGO ///////////////////////////


    chassis.turnToHeading(90, 2000);
    chassis.waitUntilDone();
    std::cout << lemlib::format_as(chassis.getPose()) << "\n";
    setIntake(127);
    
    chassis.follow(skills2_txt, 15, 3500); // pure pursuit two rings
    chassis.waitUntilDone();
    pros::delay(1000);
    ChangeLBState(PROPPED); // prop up ladybrown

    chassis.turnToHeading(-62 - 3, 2000);
    chassis.waitUntilDone();
    set_drive(26 - 2); // move to ring next to wall stake
    chassis.waitUntilDone();
    chassis.turnToHeading(0, 1500); // turn to wall stake
    chassis.waitUntilDone();
    set_drive(6.5, 750); // move to wall stake
    chassis.waitUntilDone();
    setIntake(0);
    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(600);
    set_drive(-15 - 1.5, 3000); // move back
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
    setIntake(127);
    chassis.turnToHeading(-90, 2000);
    chassis.waitUntilDone();

    set_drive(56); // intake the three rings
    chassis.waitUntil(30);
    chassis.cancelMotion();
    set_drive(26, 2000, 0, 70); // intake rings slowly
    chassis.waitUntilDone();
    chassis.turnToHeading(40, 1000);
    chassis.waitUntilDone();
    set_drive(13); // move to ring before corner
    chassis.waitUntilDone();

    chassis.turnToHeading(110, 1500);
    chassis.waitUntilDone();

    set_drive(-10); // back INto corner
    chassis.waitUntilDone();
    pros::delay(500);
    intake.move(0);
    mogoClamp.toggle(); // unclamp mogo
    pros::delay(350);

    set_drive(85.27); // go to intake ring
    chassis.waitUntil(60);
    intake.move(90);
    chassis.waitUntilDone();
    intake.move(0);
    chassis.turnToHeading(-55, 1500); // turn to third mogo
    chassis.waitUntilDone();
    set_drive(-40); // go to third mogo
    chassis.waitUntil(35);
    mogoClamp.toggle();


    /////////////////////////// THIRD MOGO ///////////////////////////
    /////////////////////////// THIRD MOGO ///////////////////////////


    chassis.waitUntilDone();
    setIntake(127);
    chassis.turnToHeading(-135, 1500); // turn to ring outside ladder
    chassis.waitUntilDone();
    set_drive(34, 2000); // move to ring outside ladder
    chassis.waitUntilDone();
    chassis.turnToHeading(-45, 1500); // turn to center ring inside ladder
    chassis.waitUntilDone();
    set_drive(30, 2000); // move to center ring inside ladder
    chassis.waitUntilDone();
    pros::delay(300);
    set_drive(-30, 2000); // move back
    chassis.turnToHeading(135, 1500);
    chassis.waitUntilDone();
    chassis.follow(skills3_txt, 15, 3000); // pure pursuit two two stacks
    chassis.waitUntilDone();
    chassis.turnToHeading(-135, 1500); // turn to third two stack
    chassis.waitUntilDone();
    set_drive(10, 2000); // intake third two stack
    chassis.waitUntilDone();
    set_drive(-10, 2000); // move back
    chassis.waitUntilDone();
    chassis.turnToHeading(-10, 1500); // turn to corner
    set_drive(-13); // move to corner
    chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    chassis.turnToHeading(-45, 1000);
    chassis.waitUntilDone();
    set_drive(13); // move out of corner
    chassis.waitUntilDone();
    chassis.turnToHeading(30, 1500);


    /////////////////////////// FOURTH MOGO ///////////////////////////
    /////////////////////////// FOURTH MOGO ///////////////////////////


    chassis.waitUntilDone();
    set_drive(80); // push other mogo to corner
    chassis.waitUntilDone();
}

void skillsMacro() {
    chassis.setPose(-60.5, -13, (-49)); // starts at middle of red alliance line
    setIntake(127); // score on alliance stake
    pros::Task lb_task(LBLoop);
    LBState = EXTENDED;
    LBRotation.set_position(4600);
    ChangeLBState(FULLEXTENDED);

    pros::delay(650);

    set_drive(-16 - 1.5, 1500, 90, 127); // move away from alliance stake
    chassis.waitUntil(12);
    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown
}