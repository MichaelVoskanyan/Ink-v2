#ifndef INK_APPLICATION_H
#define INK_APPLICATION_H

#define GLFW_INCLUDE_NONE
#include "entityManager.h"
#include "renderer/textureManager.h"
#include <GLFW/glfw3.h>
#include <entities/character.h>
#include <entities/platform.h>

#include <mutex>

/**
 * Simple application loop with window setup/cleanup.
 */
class Application {
public:
    // Access the single application instance
    static Application *getInstance();
    // Main loop: update and render
    void updateThread();
    void renderThread();

    void run();

    ~Application();

    // Delete copy operations
    Application(const Application &app) = delete;
    Application &operator=(const Application &app) = delete;

private:
    Application();

    std::mutex m_mutex;

    int width = 1280;
    int height = 720;
    const char *title = "INK";
    GLFWwindow *window = nullptr;

    // Singleton instance
    static Application *s_instance;

    // The main player character
    std::shared_ptr<Character> player = nullptr;

    EntityManager *entityManager = nullptr;
    std::shared_ptr<TextureManager> textureManager = nullptr;
};

#endif  // INK_APPLICATION_H