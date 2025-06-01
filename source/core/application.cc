// application.cpp
#include "application.h"
#include "KHR/khrplatform.h"
#include "entityManager.h"
#include "levelLoader.h"
#include <GLFW/glfw3.h>
#include <entities/character.h>
#include <entities/platform.h>
#include <glad/glad.h>
#include <iostream>
#include <renderer/buffers.h>
#include <renderer/shader.h>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <condition_variable>

Application *Application::s_instance = nullptr;
Renderer *Renderer::s_instance = nullptr;

void framebufferSizeCallback(GLFWwindow *window, int w, int h) {
    std::cout << "[application] Resized to " << w << "x" << h << "\n";
    glViewport(0, 0, w, h);
}

Application *Application::getInstance() {
    if (!s_instance) {
        s_instance = new Application();
    }
    return s_instance;
}

Application::Application() {
    std::cout << "[application] Initializing GLFW...\n";
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __MACH__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    std::cout << "[application] Creating GLFW window...\n";
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    std::cout << "[application] Loading GLAD...\n";
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        throw std::runtime_error("Failed to initialize GLAD");

    // **Make sure viewport is set once at startup**
    std::cout << "[application] Setting initial viewport to " << width << "x" << height << "\n";
    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);
    glViewport(0, 0, fbwidth, fbheight);

    // Now we can install the resize callback
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glClearColor(0.589f, 0.443f, 0.09f, 1.f);
    std::cout << "[application] Clear color set.\n";

    entityManager = EntityManager::instance();
    textureManager = TextureManager::instance();
    std::cout << "[App] textureManager = " << textureManager.get() << std::endl;
    loadLevelFromFile("assets/levels/level1.json", textureManager, entityManager);

    // // Finally, create the player (which will trigger shader loading)
    // std::cout << "[application] Creating player character...\n";
    // player = entityManager->add<Character>(glm::vec3(0.0f, 1.0f, 0.0f), 2.5f, 0.2f,
    //                                        glm::vec2(0.2f, 0.2f));
    // std::cout << "[application] Player created.\n";

    // // Create a test platform
    // std::cout << "[application] Creating test platforms...\n";
    // entityManager->add<Platform>(PlatformType::stationary, glm::vec3(0.0f, -1.0f, 0.0f), 0.0f,
    //                              glm::vec2(5.0f, 0.2f), true);
    // entityManager->add<Platform>(PlatformType::stationary, glm::vec3(0.8f, 0.0f, 0.0f), 0.0f,
    //                              glm::vec2(0.2f, 0.5f),
    //                              true);  // adding a scale to make the platform wider
    // entityManager->add<Platform>(PlatformType::stationary, glm::vec3(0.0f, -0.5f, 0.0f), 0.0f,
    //                              glm::vec2(0.5f, 0.2f),
    //                              true);  // adding a scale to make the platform wider
    // // platform->setHitboxSize(glm::vec2(5.0f, 0.5f)); will set hitboxes
    // // up later
    std::cout << "[application] Platforms created.\n";
}

Application::~Application() {
    std::cout << "[application] Terminating GLFW...\n";
    glfwDestroyWindow(window);
    glfwTerminate();
}

std::condition_variable cv;
std::atomic<bool> updated{false};
std::atomic<bool> running{true};

void Application::updateThread() {
    const float fixedDt = 1.f / 60.f;
    float accumulator = 0.0f;
    double lastTime = glfwGetTime();

    while (running) {
        double now = glfwGetTime();
        float frameT = static_cast<float>(now - lastTime);
        lastTime = now;
        accumulator += frameT;

        while (accumulator >= fixedDt) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                entityManager->update(fixedDt);
            }

            accumulator -= fixedDt;
            updated = true;
            cv.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void Application::run() {
    // target fixed physics step: 1/60th of a second
    // see https://gafferongames.com/post/fix_your_timestep/
    const float fixedDt = 1.0f / 60.0f;
    float accumulator = 0.0f;

    auto renderer = Renderer::getInstance();

    std::thread updater(&Application::updateThread, this);

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        cv.wait(lock, [] {
            return updated.load();
        });
        updated = false;
    }

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        player->handleKeyInput();
        player->handleMouseInput();

        // 1. measure real elapsed time
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            cv.wait(lock, [] {
                return updated.load();
            });
            updated = false;
        }

        glm::vec3 playerPos = player->position;  // You already have a way to track this
        glm::vec3 cameraOffset = glm::vec3(0.0f, 0.0f, 2.0f);

        // View: move the world *opposite* of the player's position
        glm::mat4 view = glm::translate(glm::mat4(1.0f), -(playerPos + cameraOffset));

        // Projection: basic perspective or orthographic
        glm::mat4 projection = glm::ortho(
            -2.0f, 2.0f,   // left, right
            -1.5f, 1.5f,   // bottom, top
            0.1f, 100.0f   // near, far
        );

        // You can also use glm::perspective for 3D view, e.g.:
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.1f, 100.0f);

        renderer->beginScene(view, projection);
        for (const auto &entity: entityManager->getEntities()) {
            if (entity->renderObject == nullptr)
                continue;  // skip entities without render objects.
            renderer->submit(entity->renderObject);
        }

        // 3. render
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // entityManager->draw();
        renderer->endScene();
        renderer->clearQueue();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    running = false;
    updater.join();
}
