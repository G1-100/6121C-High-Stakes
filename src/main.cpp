#include "main.h"

using namespace std;

double ambientHue;
long runStart;
bool testedAmbient = false;


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
	LBRotation.set_position(0);
	LBRotation.reset_position();
	pros::delay(1000);
	optical.set_led_pwm(100);
	// initializeSelector();  // Commented out selector initialization
	allianceColorBlue = false; // VERY IMPORTANT
	initColorSort();
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
        //std::cout << "RED: " << std::to_string(optical.get_rgb().red) << " BLUE: " << std::to_string(optical.get_rgb().blue) << "\n";
		//std::cout << "DIFFERENCE: " << std::to_string(optical.get_rgb().blue - optical.get_rgb().red) << "\n";
		//std::cout << "HUE: " + to_string(optical.get_hue()) << "\n";
		//std::cout << lemlib::format_as(chassis.getPose()) << "\n";
		//std::cout << ladybrown.get_actual_velocity() << "\n";
		//std::cout << LBRotation.get_position() / 100.0 << "\n";
		//std::cout << "VELOCITY: " + std::to_string(intake.get_actual_velocity()) << " VOLTAGE: " + std::to_string(intake.get_voltage()) << "\n";
		//std::cout << "PROXIMITY: " << optical.get_proximity() << " DIFFERENCE: " << std::to_string(optical.get_rgb().blue - optical.get_rgb().red) << "\n";
		//std::cout << "LED PWM" << optical.get_led_pwm() << "\n";
        pros::delay(50);
        
        // Add a way to break the loop if needed
        if (pros::competition::is_disabled()) break;
    }
}

void simpleRing(bool isBlue) {
	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, 90 * sgn);
	set_drive(-34, 2000);
	chassis.waitUntil(32 - 4);
	mogoClamp.toggle();
	chassis.waitUntilDone();
	pros::delay(1000);
	//pros::delay(500);
	intake.move_voltage(12000);
	pros::delay(700);
	chassis.turnToHeading(0, 2000);
	chassis.waitUntilDone();
	set_drive(21, 2000);
	chassis.waitUntilDone();
	pros::delay(1500);
	set_drive(-10, 1500);
	chassis.waitUntilDone();
	chassis.turnToHeading(180, 2000);
	chassis.waitUntilDone();
	set_drive(30, 2000);
	chassis.waitUntilDone();
	
}

void simpleMogo(bool isBlue) {
	int sgn=isBlue?1:-1;
	chassis.setPose(0, 0, 90 * sgn);
	set_drive(-34, 2000);
	chassis.waitUntil(32 - 4);
	mogoClamp.toggle();
	chassis.waitUntilDone();
	intake.move_voltage(12000);
	pros::delay(1000);
	chassis.turnToHeading(180, 2000);
	chassis.waitUntilDone();
	set_drive(21, 2000);
	chassis.waitUntilDone();
	pros::delay(1500);
	set_drive(-10, 1500);
	chassis.waitUntilDone();
	chassis.turnToHeading(0, 2000);
	chassis.waitUntilDone();
	set_drive(30, 2000);
	chassis.waitUntilDone();
	
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

	ColorLoopActive = false;
	intakeUnstuckActivated = true;

	//pros::Task ret4(logger);
	pros::Task lb_task(LBLoop);

	//set_drive(72);
	//intake.move(127);
	//disruptRingRush(allianceColorBlue);
	//simpleRing(allianceColorBlue);
	//mogoRushAuton(allianceColorBlue);
	//pros::Task color_task(intakeUntilColor);
	//ringAuton(allianceColorBlue);
	//MogoSideSoloAWP(allianceColorBlue);
	skills();
	//pros::delay(20000);
	//SigSoloAWP(allianceColorBlue);
	//mogoAdvayAuton(allianceColorBlue);
	//VexmenSoloAWP(allianceColorBlue);
	//simpleMogoAuton(allianceColorBlue);
	//soloAWPAutonTunedLMSD(allianceColorBlue);
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
	optical.set_led_pwm(100);
	// OP CONTROL RESET:
	brakeModeCoast();
	runStart = pros::millis();
	// INIT LADY BROWN:
	ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	LBRotation.reset_position();
	if (!LBLoopActive) { 
		pros::Task lb_task(LBLoop);
	}
	if (!colorLoopStarted) {
		//pros::Task color_task(colorSortLoop);
	}
	// Create tasks with proper handling
	pros::Task logger_task(logger);
	pros::Task temp_task(checkTemp);
	
	ColorLoopActive = true; // starts inactive until tested ambient colors

	intakeUnstuckActivated = false;
	//skillsMacro();
	// DRIVE CODE:
	while (true) {
       	// Arcade drive
		runArcadeDrive();
      	// Activate Intake Logic
		setIntakeMotors();
		//setIntakeLift();
		// Activate Doinker Logic
		setDoinker();
      	// Activate Mogo Logic
		setMogoMotors();
      	// Run Every 20 Seconds

		if (pros::millis() - runStart > 2000 && !testedAmbient) { // 2 seconds after start
			testedAmbient = true;
			ambientHue = optical.get_hue();
			activateColorSort();
			//cout << "Ambient Diff:" << optical.get_rgb().blue - optical.get_rgb().red << "ambient proximity: " << optical.get_proximity() << "\n";
		}

		pros::delay(10);
		
	}
	
	// Ensure tasks are terminated properly
    logger_task.remove();
    temp_task.remove();
}

void checkTemp() {
    std::vector<pros::Motor> motors = {driveLeftBack, driveLeftFront, driveLeftMiddle, driveRightBack, driveRightMiddle, driveRightFront};
	int count;
	double totalTemp;
    while (true) {
        totalTemp = 0.0;
        count = 0;

        for (auto& motor : motors) {
            double temp = motor.get_temperature();
            if (temp == PROS_ERR_F) { // PROS_ERR_F is returned when the motor is unplugged
                master.set_text(0, 0, "Motor " + std::to_string(motor.get_port()) + " unplugged.");
                pros::delay(250);
                master.rumble("---");
            } else if (IMU.get_heading() == NAN) {
				master.set_text(0, 0, "IMU unplugged.");
				pros::delay(250);
				master.rumble("---");
			}

            if (count < 6) {
                totalTemp += temp;
            }
            ++count;
        }

		if (master.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
			double tempFahrenheit = intake.get_temperature() * 9.0 / 5.0 + 32.0;
			master.set_text(0, 0, "Intake Temp: " + std::to_string(tempFahrenheit) + "F");
		} else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
			double tempFarenheit = ladybrown.get_temperature() * 9.0 / 5.0 + 32.0;
			master.set_text(0, 0, "LB Temp: " + std::to_string(tempFarenheit) + "F");
		} else if (count == 0) {
			master.set_text(0, 0, "No motors found.");
		} else {
			double averageTempCelsius = totalTemp / count;
			double averageTempFahrenheit = averageTempCelsius * 9.0 / 5.0 + 32.0;
			master.set_text(0, 0, "Avg Temp: " + std::to_string(averageTempFahrenheit) + "F");
		}
		pros::delay(500);
    }
}