#ifndef INK_APPLICATION_H
#define INK_APPLICATION_H

/**
 * Simple application loop with window setup/cleanup.
 */

struct GLFWwindow;

class application_t {
    int width = 1920, height = 1080;
    const char *title = "INK";
    GLFWwindow *window;
    
    static application_t *s_instance;

    application_t();

public:
    application_t(const application_t &app) = delete;
    application_t &operator=(const application_t &app) = delete;

    static application_t *getInstance();

public:
    void run();
};
#endif