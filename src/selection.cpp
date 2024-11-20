#include "selection.hpp"

Selection::Selection(bool isBlue) : color(isBlue), selectedAuton(0) {}

void Selection::registerAuton(const std::string& name, const std::string& description, std::function<void()> autonFunction) {
    autons.push_back({name, description, autonFunction});
}

void Selection::handleSelection() {
    while (true) {
        if (pros::lcd::read_buttons() == LCD_BTN_LEFT) {
            selectedAuton = (selectedAuton - 1 + autons.size()) % autons.size(); // Cycle through autons
            displayMenu();
            pros::delay(300); // Debounce delay
        } else if (pros::lcd::read_buttons() == LCD_BTN_RIGHT) {
            selectedAuton = (selectedAuton + 1) % autons.size(); // Cycle through autons
            displayMenu();
            pros::delay(300); // Debounce delay
        } else if (pros::lcd::read_buttons() == LCD_BTN_CENTER) {
            color = !color; // Flip alliance color
            displayMenu();
            pros::delay(300); // Debounce delay
        }
        pros::delay(20);
    }
}

void Selection::displayMenu() {
    pros::lcd::clear();
    pros::lcd::set_text(1, "Auton: " + std::get<0>(autons[selectedAuton]));
    pros::lcd::set_text(2, "Desc: " + std::get<1>(autons[selectedAuton]));
    pros::lcd::set_text(3, "Alliance: " + std::string(color ? "Blue" : "Red"));
}