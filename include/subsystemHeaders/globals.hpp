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


extern pros::Optical optical;

extern bool allianceColorBlue;

//intake
extern pros::Motor intake;

//ladybrown
extern pros::Motor ladybrown;


// Inertial
extern pros::Imu IMU;

// Rotational Sensor
extern pros::Rotation horizTracking;
extern pros::Rotation vertTracking;

extern pros::Rotation LBRotation;

extern lemlib::TrackingWheel horizTrackingWheel;
extern lemlib::TrackingWheel vertTrackingWheel;

//CONTROLLER
extern  pros::Controller master;


//MISCELLANEOUS

extern pros::adi::Pneumatics intakeLift;
extern pros::adi::Pneumatics doinker;
extern pros::adi::Pneumatics mogoClamp;
pros::adi::Pneumatics rushRightPiston;
pros::adi::Pneumatics rushLeftPiston;

//extern pros::adi::Port sensor;

extern lemlib::Chassis chassis;
extern lemlib::OdomSensors sensors;
