#include "main.h"
#include "pros/llemu.hpp"
#include "pros/rotation.h"
#include "subsystemHeaders/globals.hpp"
#include "subsystemHeaders/odometry.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <array>
#include <iostream>
#include <string.h>

using namespace std;


// all measurements are done in INCHES!

const float DRIVE_WHEEL_DIAMETER = 3.25;
const float TRACK_WHEEL_DIAMETER = 2.75;


const float DRIVE_RATIO = 2.0/3;

const double TRACK_DIAMETER = 11.5; // distance from left side wheels and right side wheels
const double TR = TRACK_DIAMETER/2; //disntace from right wheel to tracking center
const double TL = TRACK_DIAMETER/2; //distance from left wheel to tracking center
const double TB = 0; //distance from back wheel to tracking center

const double TRACKING_WHEEL_CIRCUMFERENCE = M_PI * TRACK_WHEEL_DIAMETER;
const double DRIVE_WHEEL_CIRCUMFERENCE = M_PI * DRIVE_WHEEL_DIAMETER;


double degreesToRadConversion = M_PI/180;
double radToDegreesConversion = 1.0/degreesToRadConversion;

const double DRIVE_DEGREE_TO_INCH = DRIVE_RATIO / 360 * DRIVE_WHEEL_CIRCUMFERENCE;
const double TRACKING_WHEEL_DEGREE_TO_INCH = TRACKING_WHEEL_CIRCUMFERENCE / 360;


// these functions return how far each tracker has travled in inches
// using the average of the left and right motor encoders
double getLeftEncoders(){
  /* return   (driveLeftFront.get_position() +
            driveLeftMiddle.get_position() +
            driveLeftBack.get_position()) /(double) -3 * DRIVE_DEGREE_TO_INCH; // LEFT ENCODERS ARE REVERSE
        */
    return -driveLeftMiddle.get_position() * DRIVE_DEGREE_TO_INCH;
}

double getRightEncoders(){
   /* return   (driveRightFront.get_position() +
            driveRightMiddle.get_position() +
            driveRightBack.get_position()) /(double) 3 * DRIVE_DEGREE_TO_INCH; // RIGHT ENCODERS ARE STANDARD
            */
    return driveRightMiddle.get_position() * DRIVE_DEGREE_TO_INCH;
}

// using 1 horizantal tracking wheel
double getBackEncoder() {
    return horizTracking.get_position() /(double) 100 * TRACKING_WHEEL_DEGREE_TO_INCH; // returns centidegrees so divided by 100
}

void resetEncoders() {
    driveLeftBack.tare_position();
    driveLeftMiddle.tare_position();
    driveLeftFront.tare_position();
    driveRightBack.tare_position();
    driveRightMiddle.tare_position();
    driveRightFront.tare_position();

}


// Odom code once we have multithreading to run the Odom function continuously in the background- 


// need to set values for the odom[] array at the start of autonomous to give starting position
// rotation sensors will DRIFT over time so reset them or recalibrate them (maybe with GPS) once in a while (the resetting is not done in the code)

double odom[] = {0, 0, 0}; // odom starting values are updated at the start of auton based on which routine I choose and position of the robot, origin of the robot is always the same

void odomIteration(double dLeft, double dRight, double dHorizantal){
    // formula for dTheta uses the change in left, right, and horizantal distances that the bot has travled
    // double dTheta = -(dLeft - dRight) / (TL + TR);
    double dTheta = (360 - IMU.get_heading()) * degreesToRadConversion - odom[2];
    pros::lcd::set_text(1, "dTheta: " + std::to_string(dTheta) + "IMU: " + std::to_string(IMU.get_heading()));
    double s, r;

    if(dTheta == 0){
        // if the robot Theta doesn't change, 1) the robot moved directly straight and horizantal distance traveled by the bot is "s"
        // and 2) the distance that the left or right side of the bot has traveled (both are same) equals the distance traveled forwards, "r"
        s = dHorizantal;
        r = dLeft;
    }
    else{
        // if there is a change in Theta, use the following formulas to calculate "s" and "r"  -> robot's coordinates (without taking midTheta into consideration)
        s = 2 * (dHorizantal / dTheta + TB) * sin(dTheta / 2);
        r = 2 * (dRight / dTheta + TR) * sin(dTheta / 2);
    }

    // calculate the robot's change in x and y coordinates using midTheta (average orientation)
    double midTheta = odom[2] + dTheta / 2;
    double dx = s * cos(midTheta) + r * sin(midTheta);
    double dy = s * -sin(midTheta) + r * cos(midTheta);
    
    // add up the dx, dy, and dTheta values after every iteration to get the absolute x and y coordinates, and absolute orientation

    odom[0] = odom[0] + dx;
    odom[1] = odom[1] + dy;
    odom[2] = odom[2] + dTheta;


    pros::lcd::set_text(2, "X: " + std::to_string(odom[0]));
    pros::lcd::set_text(3, "Y: " + std::to_string(odom[1]));
    pros::lcd::set_text(4, "Heading: " + std::to_string(odom[2] * radToDegreesConversion));
}



void odometry(){
    

    static double prevLeft = 0; 
    static double prevRight = 0;
    static double prevHorizantal = 0;

    odom[2] = 0;
    //pros::lcd::set_text(1, "theta: " + std::to_string(odom[2]));

    // prevLeft, prevRight, and prevHorizantal below are used to show how much the bot had traveled in the previous cycle

    // Subtract previous values from how much the bot has traveled in the current cycle (left, right, and horizantal) to calculate the change in distance traveled by each tracker (dLeft, dRight, and dMid)
    while(true){
        // the getEncoder functions output how far each tracking wheel has traveled in inches
        double left = getLeftEncoders();
        double right = getRightEncoders();
        double horizantal = getBackEncoder();

        // change (dLeft, dRight, and dHorizantal) is inputted as parameters of odomIteration
        odomIteration(left - prevLeft, right - prevRight, horizantal - prevHorizantal);

        // update the previous values for the next cycle
        prevLeft = left;
        prevRight = right;
        prevHorizantal = horizantal;
        /*
        if (odom[2]*radToDegreesConversion >= 360) {
            odom[2] = odom[2] - 2*M_PI;
        }
        else if (odom[2]*radToDegreesConversion <= -360) {
            odom[2] = odom[2] + 2*M_PI;
        }
        
        if (odom[2]*radToDegreesConversion < 0) {
            odom[2] = odom[2] + 2*M_PI;
        }
        */
        pros::delay(10);

    }
}

