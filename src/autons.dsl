// API Definitions
api
    chassis.setPose(x, y, theta); pose x y theta
    LBExtend(x); LBExtend x
    mogoClamp.toggle(); Mogo
    set_drive(in, ms, min, max); Drive in ms=3000 min=0 max=127
    chassis.turnToPoint(x, y, ms, {.forwards=forward, .minSpeed=min, .maxSpeed=max}); TurnTo x y ms=3000 forward=true min=0 max=127
    chassis.setPose(x, y, theta); pose x y theta
end

// Autonomous Routines
routine mogoAdvayAuton
    pose 48 60 90
    Drive 21
    Mogo
    TurnTo 27 -20 3000 false
    Drive 27 1500 30
    Mogo
    LBExtend 1
end

routine ringAuton
    pose 60 -27.5 -90
    // Add ring auton commands here
end

routine skillsAuton
    pose -63 0 90
    // Add skills auton commands here
end