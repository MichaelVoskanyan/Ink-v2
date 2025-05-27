// entry.cc
#include "application.h"
#include <iostream>
#include <exception>

int main() {
    try {
        Application::getInstance()->run();
    } catch (const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return -1;
    }
    return 0;
}
