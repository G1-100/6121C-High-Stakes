#include "main.h"
#include <vector>
#include <utility>
#include <fstream>

struct Command {
    int left;
    int right;
    bool mogoClamp;
    bool lbextend;
    bool intakeLift;
    bool intake;
};

std::vector<Command> recordedCommands;
bool isRecording = false;

void startRecording() {
    recordedCommands.clear();
    isRecording = true;
}

void stopRecording() {
    isRecording = false;
}

void replayCommands() {
    for (const auto& command : recordedCommands) {
        setDrive(command.left, command.right);
        if (command.mogoClamp) mogoClamp.toggle();
        if (command.lbextend) ChangeLBState(EXTENDED);
        if (command.intakeLift) setIntakeLift(); else setIntakeLift();
        if (command.intake) setIntake(127); else setIntake(0);
        pros::delay(20); // Adjust delay as needed
    }
}

void saveCommandsToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    for (const auto& command : recordedCommands) {
        file.write(reinterpret_cast<const char*>(&command), sizeof(Command));
    }
    file.close();
}

void loadCommandsFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    Command command;
    while (file.read(reinterpret_cast<char*>(&command), sizeof(Command))) {
        recordedCommands.push_back(command);
    }
    file.close();
}

void lbLoop() {
    while (true) {
        // Implement lbLoop logic here based on ladybrown.cpp
        // Example: ChangeLBState(REST);
        pros::delay(20);
    }
}

void opcontrol() {
    startRecording();
    pros::Task lb_task(LBLoop);
    
    while (true) {
        setIntakeLift();
        setIntakeMotors();
        setMogoMotors();
        int leftJoystick = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightJoystick = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        int left = leftJoystick + rightJoystick;
        int right = leftJoystick - rightJoystick;
        bool mogoClampState = master.get_digital(pros::E_CONTROLLER_DIGITAL_R1);
        bool lbextendState = master.get_digital(pros::E_CONTROLLER_DIGITAL_L1);
        bool intakeLiftState = master.get_digital(pros::E_CONTROLLER_DIGITAL_B);
        bool intakeState = master.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
        
        setDrive(left, right);
        
        if (isRecording) {
            recordedCommands.push_back({left, right, mogoClampState, lbextendState, intakeLiftState, intakeState});
        }
        pros::delay(20);
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_X)) {
            stopRecording();
            saveCommandsToFile("/usd/commands.dat");
            break;
        }
    }
}

void autonomous() {
    loadCommandsFromFile("/usd/commands.dat");
    replayCommands();
}
