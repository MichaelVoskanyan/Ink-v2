// entry.cc
#include "application.h"
#include <iostream>
#include <exception>

int main() {
    try {
        application_t::getInstance()->run();
    }
    catch(const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return -1;
    }
    return 0;
}
