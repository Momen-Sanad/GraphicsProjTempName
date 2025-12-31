#pragma once
#include <string>
#include <iostream>

class Logger {
public:
    static void log(const std::string& message) {
        std::cout << "[LOG] " << message << std::endl;
    }
};
