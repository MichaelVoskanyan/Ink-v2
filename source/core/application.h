#ifndef INK_APPLICATION_H
#define INK_APPLICATION_H

#define GLFW_INCLUDE_NONE
#include "./core/entityManager.h"
#include <GLFW/glfw3.h>
#include <entities/character.h>
#include <entities/platform.h>

/**
 * Simple application loop with window setup/cleanup.
 */
class application_t {
public:
  // Access the single application instance
  static application_t *getInstance();
  // Main loop: update and render
  void run();

  ~application_t();

  // Delete copy operations
  application_t(const application_t &app) = delete;
  application_t &operator=(const application_t &app) = delete;

private:
  application_t();

  int width = 1920;
  int height = 1080;
  const char *title = "INK";
  GLFWwindow *window = nullptr;

  // Singleton instance
  static application_t *s_instance;

  // The main player character
  std::shared_ptr<character_t> player = nullptr;
  entityManager_t *entityManager = nullptr;
};

#endif // INK_APPLICATION_H