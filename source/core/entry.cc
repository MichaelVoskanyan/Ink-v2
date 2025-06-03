// entry.cc
#include "application.h"
#include <cstdio>
#include <exception>

int main() {
    try {
        Application::getInstance()->run();
    } catch (const std::exception &e) {
//        std::cerr << "Unhandled exception: " << e.what() << "\n";
        printf("Unhandled exception: %s\n", e.what() );
        return -1;
    }
    return 0;
}
