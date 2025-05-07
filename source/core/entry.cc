#include "application.h"


int main() {
    application_t *app = application_t::getInstance();
    app->run();
    delete app;
}
