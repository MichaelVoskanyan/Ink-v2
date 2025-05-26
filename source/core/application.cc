// application.cpp
#include "application.h"
#include "entityManager.h"
#include <GLFW/glfw3.h>
#include <entities/character.h>
#include <entities/platform.h>
#include <glad/glad.h>
#include <iostream>
#include <renderer/buffers.h>
#include <renderer/shader.h>
#include <stdexcept>

application_t *application_t::s_instance = nullptr;

void framebufferSizeCallback(GLFWwindow *window, int w, int h) {
  std::cout << "[application] Resized to " << w << "x" << h << "\n";
  glViewport(0, 0, w, h);
}

application_t *application_t::getInstance() {
  if (!s_instance) {
    s_instance = new application_t();
  }
  return s_instance;
}

application_t::application_t() {
  std::cout << "[application] Initializing GLFW...\n";
  if (!glfwInit())
    throw std::runtime_error("Failed to initialize GLFW");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  std::cout << "[application] Creating GLFW window...\n";
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!window)
    throw std::runtime_error("Failed to create GLFW window");

  glfwMakeContextCurrent(window);

  std::cout << "[application] Loading GLAD...\n";
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    throw std::runtime_error("Failed to initialize GLAD");

  // **Make sure viewport is set once at startup**
  std::cout << "[application] Setting initial viewport to " << width << "x"
            << height << "\n";
  glViewport(0, 0, width, height);

  // Now we can install the resize callback
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glClearColor(0.589f, 0.443f, 0.09f, 1.f);
  std::cout << "[application] Clear color set.\n";

  entityManager = entityManager_t::instance();

  // Finally, create the player (which will trigger shader loading)
  std::cout << "[application] Creating player character...\n";
  player = entityManager->add<character_t>(glm::vec3(0.0f, 1.0f, 0.0f), 2.5f,
                                           0.2f, glm::vec2(0.2f, 0.2f));
  std::cout << "[application] Player created.\n";

  // Create a test platform
  std::cout << "[application] Creating test platforms...\n";
  entityManager->add<platform_t>(platformType_e::stationary,
                                 glm::vec3(0.0f, -1.0f, 0.0f), 0.0f,
                                 glm::vec2(5.0f, 0.2f), true);
  entityManager->add<platform_t>(
      platformType_e::stationary, glm::vec3(0.8f, 0.0f, 0.0f), 0.0f,
      glm::vec2(0.2f, 0.5f),
      true); // adding a scale to make the platform wider
  entityManager->add<platform_t>(
      platformType_e::stationary, glm::vec3(0.0f, -0.5f, 0.0f), 0.0f,
      glm::vec2(0.5f, 0.2f),
      true); // adding a scale to make the platform wider
  // platform->setHitboxSize(glm::vec2(5.0f, 0.5f)); will set hitboxes
  // up later
  std::cout << "[application] Platforms created.\n";
}

application_t::~application_t() {
  std::cout << "[application] Terminating GLFW...\n";
  glfwDestroyWindow(window);
  glfwTerminate();
}

void application_t::run() {
  // target fixed physics step: 1/60th of a second
  // see https://gafferongames.com/post/fix_your_timestep/
  const float fixedDt = 1.0f / 60.0f;
  float accumulator = 0.0f;

  double lastTime = glfwGetTime();
  while (!glfwWindowShouldClose(window)) {
    // 1. measure real elapsed time
    double now = glfwGetTime();
    float frameT = static_cast<float>(now - lastTime);
    lastTime = now;

    // 2. accumulate and step physics in fixed increments
    accumulator += frameT;
    while (accumulator >= fixedDt) {
      // process all your physics/logic at a steady 60 Hz
      entityManager->update(fixedDt);
      accumulator -= fixedDt;
    }

    // 3. render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    entityManager->draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
