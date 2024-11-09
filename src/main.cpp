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


void setDoinker() {
    // if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2)) {
    //     doinker.toggle();
    // }
}

void logger() {
	while (true) {

		cout << "ANGLE: " + std::to_string(LBRotation.get_position() / 100) << "\n";
		pros::delay(100);
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

	//pros::Task ret1(LBLoop);
	//LBRotation.reset();

	driveLeftBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveLeftMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveLeftFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightBack.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightMiddle.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	driveRightFront.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);

	// Debug: pros::Task ret4(logger);
	//simpleSkills();
	//simpleMogoAuton(true);
	ringAuton(true);
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
	// Set Brake Mode to Coast
	brakeModeCoast();
	// INIT LADY BROWN:
	ladybrown.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
	LBRotation.reset_position();
	if (!LBLoopActive) { 
		pros::Task ret2(LBLoop); 
	}
	pros::Task ret4(logger);
	
	// Check Temperature of Motors
	pros::Task temp(checkTemp);
  
	// DRIVE CODE:
	while (true) {
       	// Arcade drive
		runArcadeDrive();
      	// Activate Intake Logic
		setIntakeMotors();
		// Activate Doinker Logic
		setDoinker();
      	// Activate Mogo Logic
		setMogoMotors();
      	// Run Every 20 Seconds
		pros::delay(20);
	}
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