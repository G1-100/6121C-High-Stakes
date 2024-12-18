#include "main.h"
#include "pros/motors.h"
#include "lemlib/api.hpp"



//MOTORS

//drive
pros::Motor driveLeftBack(-20, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveLeftFront(-2, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveLeftMiddle(-3, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightBack(10, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightFront(4, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::Motor driveRightMiddle(9, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

//intake
pros::Motor intake(1, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

// ladybrown wall stake mech
pros::Motor ladybrown(-6, pros::v5::MotorGears::red, pros::v5::MotorUnits::degrees);



//pistons
pros::adi::Pneumatics intakeLift('D', false);
pros::adi::Pneumatics doinker('B', false);
pros::adi::Pneumatics mogoClamp('H', false);

// inertial
pros::Imu IMU(8);

// rotational sensor
pros::Rotation horizTracking(12);
pros::Rotation vertTracking(19);

bool allianceColorBlue = true;

pros::Optical optical(18);

pros::Rotation LBRotation(7);

//CONTROLLERS
pros::Controller master(pros::E_CONTROLLER_MASTER);

pros::MotorGroup left_side_motors({-20, -2, -3}, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);
pros::MotorGroup right_side_motors({10, 4, 9}, pros::v5::MotorGears::blue, pros::v5::MotorUnits::degrees);

	lemlib::Drivetrain drivetrain(
		&left_side_motors, 
		&right_side_motors, 
		12, // track width
		lemlib::Omniwheel::NEW_325, // wheel diameter
		450, // rpm
		6 // chase power
	);

    lemlib::ControllerSettings lateral_controller(
        5.5, // proportional gain (kP)
        0, // integral gain (kI)
        15, // derivative gain (kD)
        3, // anti windup
        1, // small error range, in inches
        100, // small error range timeout, in milliseconds
        3, // large error range, in inches
        500, // large error range timeout, in milliseconds
        20 // maximum acceleration (slew)
    );

    lemlib::ControllerSettings angular_controller(6, // proportional gain (kP) safe: 2.5, 4.5, 6
        0, // integral gain (kI)
        48, // derivative gain (kD) 14, 32, 48
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