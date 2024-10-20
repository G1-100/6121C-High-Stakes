#pragma once
#include "main.h"
#include "pros/adi.hpp"
#include "pros/misc.h"
#include "pros/rotation.hpp"
#include "lemlib/api.hpp"

//MOTORS

//drive
extern pros::Motor driveLeftBack;
extern pros::Motor driveLeftMiddle;
extern pros::Motor driveLeftFront;
extern pros::Motor driveRightBack;
extern pros::Motor driveRightMiddle;
extern pros::Motor driveRightFront;

extern pros::MotorGroup left_side_motors;
extern pros::MotorGroup right_side_motors;

//intake
extern pros::Motor intake;


// Inertial
extern pros::Imu IMU;

// Rotational Sensor
extern pros::Rotation horizTracking;
extern pros::Rotation vertTracking;

extern lemlib::TrackingWheel horizTrackingWheel;
extern lemlib::TrackingWheel vertTrackingWheel;

//CONTROLLER
extern  pros::Controller master;


//MISCELLANEOUS

extern pros::adi::DigitalOut piston1;

extern lemlib::Chassis chassis;
extern lemlib::OdomSensors sensors;
