#include "main.h"
#include "pros/motors.h"
#include "lemlib/api.hpp"



//MOTORS

//drive
pros::Motor driveLeftBack(1, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveLeftFront(-3, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveLeftMiddle(-2, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightBack(11, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightFront(-13, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightMiddle(12, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

//intake
pros::Motor intake(9, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

// ladybrown wall stake mech
pros::Motor ladybrown(-8, pros::v5::MotorGears::red, pros::v5::MotorUnits::degrees);

//pistons
pros::adi::Pneumatics intakeLift('X', false);
pros::adi::Pneumatics doinker('X', false); 
pros::adi::Pneumatics mogoClamp('D', false);
pros::adi::Pneumatics rushRightPiston('C', false);
pros::adi::Pneumatics rushLeftPiston('B', false);

// inertial
pros::Imu IMU(4);

bool allianceColorBlue = true;

// rotational sensor
pros::Rotation horizTracking(12);
pros::Rotation vertTracking(19);

pros::Optical optical(18);

pros::Rotation LBRotation(7);

//CONTROLLERS
pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup left_side_motors({1, -3, -2}, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::MotorGroup right_side_motors({11, -13, 12}, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

	lemlib::Drivetrain drivetrain(
		&left_side_motors, 
		&right_side_motors, 
		12.5, // track width
		lemlib::Omniwheel::NEW_275, // wheel diameter
		450, // rpm
        8 // horizontal drift
	);

    lemlib::ControllerSettings lateral_controller(
        11, // proportional gain (kP)
        0, // integral gain (kI)
        45 + 10, // derivative gain (kD)
        3, // anti windup
        1, // small error range, in inches
        100, // small error range timeout, in milliseconds
        3, // large error range, in inches
        500, // large error range timeout, in milliseconds
        0 // maximum acceleration (slew)
    );

    lemlib::ControllerSettings angular_controller(  
        7, // proportional gain (kP) safe: 3
        0, // integral gain (kI)
        55, // derivative gain (kD) 20
        3, // anti windup
        1, // small error range, in inches
        100, // small error range timeout, in milliseconds
        2, // large error range, in inches
        500, // large error range timeout, in milliseconds
        0 // maximum acceleration (slew)
    );

    lemlib::TrackingWheel horizTrackingWheel(&horizTracking, lemlib::Omniwheel::NEW_275, -5.75);
    lemlib::TrackingWheel vertTrackingWheel(&vertTracking, lemlib::Omniwheel::NEW_275, -2.5);

	lemlib::OdomSensors sensors(nullptr,//&vertTrackingWheel, // vertical tracking wheel 1
        nullptr, // vertical tracking wheel 2, set to nullptr as we are using IMEs
        nullptr,//&vertTrackingWheel, // horizontal tracking wheel 1
        nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
        &IMU // inertial sensor
    );

	lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller, sensors);