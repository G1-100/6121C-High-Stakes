#pragma once
#include "main.h"
#include <vector>
#include <array>
#include <iostream>
using namespace std;

extern double degreesToRadConversion;
extern double radToDegreesConversion;
double getLeftEncoders();
double getRightEncoders();
double getBackEncoder();
void resetEncoders();
extern double odom[3];
extern double dTheta;
void odomIteration();
void odometry();

