#include "main.h"

using namespace std;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");
	chassis.calibrate();
	LBRotation.set_position(RESTANGLE);
	pros::delay(1000);
	// initializeSelector();  // Commented out selector initialization
	allianceColorBlue = true; // VERY IMPORTANT
	//initColorSort();

}


/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

void logger() {
    while (!pros::competition::is_disabled()) {
        //std::cout << "HUE: " << std::to_string(optical.get_hue()) << "\n";
        pros::delay(100);
        
        // Add a way to break the loop if needed
        if (pros::competition::is_disabled()) break;
    }
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    // Remove direct call to ringAuton and only use selector
    // selector->runSelectedAutonomous();  // Commented out selector usage
	driveLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveLeftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	left_side_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	right_side_motors.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	// Debug: pros::Task ret4(logger);
	//simpleSkills();
	//simpleMogoAuton(true);
	//ringAuton(allianceColorBlue);
	//soloAWPAutonTunedLMSD(true);
	//mogoAdvayAuton(allianceColorBlue);
	//LMSDSkills();
	VexmenSoloAWP(true);
	//chassis.waitUntilDone();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	// OP CONTROL RESET:
	brakeModeCoast();
	// INIT LADY BROWN:
	ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	LBRotation.reset_position();
	if (!LBLoopActive) { 
		pros::Task lb_task(LBLoop);
	}
	// Create tasks with proper handling
	pros::Task logger_task(logger);
	pros::Task temp_task(checkTemp);
  
	// DRIVE CODE:
	while (true) {
       	// Arcade drive
		runArcadeDrive();
      	// Activate Intake Logic
		setIntakeMotors();
		setIntakeLift();
		// Activate Doinker Logic
		setDoinker();
      	// Activate Mogo Logic
		setMogoMotors();
      	// Run Every 20 Seconds
		pros::delay(10);
		
	}
	
	// Ensure tasks are terminated properly
    logger_task.remove();
    temp_task.remove();
}

void checkTemp() {
    std::vector<pros::Motor> motors = {driveLeftBack, driveLeftFront, driveLeftMiddle, driveRightBack, driveRightMiddle, driveRightFront};

    while (true) {
        double totalTemp = 0.0;
        int count = 0;

        for (auto& motor : motors) {
            double temp = motor.get_temperature();
            if (temp == PROS_ERR_F) { // PROS_ERR_F is returned when the motor is unplugged
                master.set_text(0, 0, "Motor " + std::to_string(motor.get_port()) + " unplugged.");
                pros::delay(250);
                master.rumble("---");
            }

            if (count < 6) {
                totalTemp += temp;
            }
            ++count;
        }

        if (count == 0) master.set_text(0, 0, "No motors found.");

        double averageTempCelsius = totalTemp / count;
        double averageTempFahrenheit = averageTempCelsius * 9.0 / 5.0 + 32.0;
        master.set_text(0, 0, "Avg Temp: " + std::to_string(averageTempFahrenheit) + "F");
        pros::delay(500);
    }
}