#pragma once

#include "main.h"
#include <vector>
#include <functional>

class Selection {
    public:
        Selection(bool isBlue);
        void handleSelection();
        void displayMenu();
        void registerAuton(const std::string& name, const std::string& description, std::function<void()> autonFunction);
    private:
        bool color;
        int selectedAuton;
        std::vector<std::tuple<std::string, std::string, std::function<void()>>> autons;
};