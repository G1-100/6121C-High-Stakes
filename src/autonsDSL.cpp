void mogoAdvayAuton() {
    chassis.setPose(48, 60, 90);
    set_drive(21, 3000, 0, 127);
    chassis.waitUntilDone();
    mogoClamp.toggle();
    chassis.turnToPoint(27, -21, 3000, {.forwards=false, .minSpeed=0, .maxSpeed=127});
    chassis.waitUntilDone();
    set_drive(27, 1500, 30, 127);
    chassis.waitUntilDone();
    mogoClamp.toggle();
    LBExtend(1);
}

void ringAuton() {
    chassis.setPose(60, -27.5, -90);
}

void skillsAuton() {
    chassis.setPose(-63, 0, 90);
}
