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

    pros::delay(1000);

    set_drive(-16 - 1.5); // move away from alliance stake
    chassis.waitUntil(11);
    mogoClamp.toggle(); // clamp mogo
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    // chassis.turnToPoint(-50, -18, 3000, {.forwards = false}); // move to mogo
    // chassis.waitUntilDone();
    // set_drive(-18);
    // chassis.waitUntilDone();

    // mogoClamp.toggle(); // clamp mogo

    //chassis.turnToPoint(-22, -24 + 3, 3000);
    chassis.turnToHeading(90, 2000);
    chassis.waitUntilDone();

    chassis.follow(skills1_txt, 15, 3000); // pure pursuit two rings
    chassis.waitUntilDone();
    pros::delay(1000);
    ChangeLBState(PROPPED); // prop up ladybrown
    //chassis.turnToPoint(-8, -53, 3000); // turn to go to neutral wall stake
    chassis.turnToHeading(-115 + 3, 2000);
    chassis.waitUntilDone();

    // chassis.moveToPoint(-8, -53, 3000); // pickup ring next to wall stake
    // chassis.waitUntilDone();

    set_drive(28); // pickup ring next to wall stake
    chassis.waitUntilDone();

    pros::delay(900);
    chassis.turnToHeading(180, 3000);
    chassis.waitUntilDone();
    // chassis.moveToPoint(-7 - 1, -60, 3000); // move to wall stake

    set_drive(5, 1000); // move to wall stake
    chassis.waitUntilDone();

    intake.move(0);
    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(600);
    // chassis.moveToPoint(-8, -50, 3000, {.forwards = false}); // go back a bit

    set_drive(-13.5); // go back a bit
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    //chassis.turnToPoint(-61, -48, 3000);
    chassis.turnToHeading(-90, 1500);
    setIntake(127);
    chassis.waitUntilDone();
    // chassis.moveToPoint(-61, -48, 3000); // collect two bottom-left rings

    set_drive(55 + 4, 3000); // go slow collecting 3 rings
    chassis.waitUntilDone();
    pros::delay(1000);
    //chassis.turnToPoint(-47, -58, 3000); // turn to last bottom-left ring
    chassis.turnToHeading(135, 2000);
    chassis.waitUntilDone();
    // chassis.moveToPoint(-47, -58, 3000); // collect last bottom-left ring
    
    set_drive(15); // collect last bottom-left ring
    chassis.waitUntilDone();
    pros::delay(600);
    //chassis.turnToPoint(-57.2, -57.2, 1000, {.forwards = false});
    chassis.turnToHeading(75, 2000);
    chassis.waitUntilDone();
    //chassis.moveToPoint(-57.2, -57.2, 500, {.forwards = false}); // move to corner

    set_drive(-13); // move to corner
    chassis.waitUntilDone();
    // chassis.turnToPoint(-60,-60, 500, {.forwards=false}); // look to center
    // chassis.waitUntilDone();
    mogoClamp.toggle(); // unclamp mogo
    pros::delay(500);

    //chassis.moveToPoint(-48+2, -48, 3000); // move out of corner

    set_drive(8.5); // move out of corner
    chassis.waitUntilDone();
    //chassis.turnToPoint(-48,22,3000, {.forwards=false});
    chassis.turnToHeading(180, 1500);
    chassis.waitUntilDone();
    setIntake(0);
    //chassis.moveToPoint(-48,22 + 4, 3000, {.forwards=false}); // move to mogo

    set_drive(-80 - 8); // move to mogo
    chassis.waitUntilDone();
    mogoClamp.toggle(); // clamp mogo
    //chassis.turnToPoint(-20, 24 + 2, 3000);
    chassis.turnToHeading(90, 2000);
    chassis.waitUntilDone();
    setIntake(127);
    
    chassis.follow(skills2_txt, 15, 3000); // pure pursuit two rings
    chassis.waitUntilDone();
    pros::delay(1000);
    ChangeLBState(PROPPED); // prop up ladybrown

    //chassis.moveToPoint(-20, 26, 3000); // move to ring outside ladder

    // set_drive(24); // move to ring outside ladder
    // chassis.waitUntilDone();
    // pros::delay(700);
    // //chassis.moveToPoint(-24, 24, 3000, {.forwards = false}); // move back

    // set_drive(-4.47); // move back
    // chassis.waitUntilDone();
    // chassis.turnToPoint(-3, 55, 3000);
    // chassis.waitUntilDone();
    // //chassis.moveToPoint(0, 61, 3000); // move to ring next to wall stake

    set_drive(26); // move to ring next to wall stake
    chassis.waitUntilDone();
    chassis.turnToHeading(0, 1500); // turn to wall stake
    chassis.waitUntilDone();
    set_drive(5); // move to wall stake
    chassis.waitUntilDone();
    ChangeLBState(EXTENDED); // extend ladybrown
    pros::delay(600);
    set_drive(-8, 3000); // move back
    chassis.waitUntilDone();
    ChangeLBState(REST); // retract ladybrown

    //chassis.turnToPoint(-26, 46 + 3, 3000);
    chassis.turnToHeading(-90, 2000);
    chassis.waitUntilDone();
    //chassis.moveToPoint(-26, 50 - 1, 3000); // move to ring outside ladder

    set_drive(60); // intake the three rings
    chassis.waitUntilDone();
    pros::delay(700);
    //chassis.moveToPoint(-24, 49, 3000, {.forwards=false});
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