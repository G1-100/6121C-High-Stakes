#pragma once
#include "main.h"
#include "lemlib/api.hpp"

// Helper Functions
void setDrive(int left, int right);
void setDriveVelocity(int left, int right);
void setDriveVoltage(int left, int right);
double averageDriveEncoderValues();
void resetDriveEncoders();

// Driver Control Functions
void setDriveMotors();

void brakeModeCoast();

// Autonomous Functions
void translate(int units, int voltage);

void rotate(int degrees, int voltage);

void runArcadeDrive();

void runCheesyDrive();

static double _turnRemapping(double iturn);

static void _updateAccumulators(double& quickStopAccumlator, double& negInertiaAccumlator);

std::pair<double, double> cheesyDrive(double ithrottle, double iturn, double prevTurn, double prevThrottle, double& quickStopAccumlator, double& negInertiaAccumlator);