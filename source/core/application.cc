// application.cpp
#include "application.h"
#include "KHR/khrplatform.h"
#include "entityManager.h"
#include "levelLoader.h"
#include <GLFW/glfw3.h>
#include <entities/character.h>
#include <entities/platform.h>
#include <entities/canvasOverlay.h>
#include <glad/glad.h>
#include <iostream>
#include <renderer/buffers.h>
#include <renderer/shader.h>
#include <stdexcept>
#include <thread>
#include <atomic>
#include <algorithm>
#include <condition_variable>
#include "strokeRecorder.h"
#include "recognizer.h"

Application *Application::s_instance = nullptr;
Renderer *Renderer::s_instance = nullptr;

namespace {
struct StrokeBounds {
    glm::vec2 min;
    glm::vec2 max;
};

StrokeBounds boundsFromPoints(const std::vector<glm::vec2>& points) {
    StrokeBounds bounds{points.front(), points.front()};
    for (const auto& p : points) {
        bounds.min.x = std::min(bounds.min.x, p.x);
        bounds.min.y = std::min(bounds.min.y, p.y);
        bounds.max.x = std::max(bounds.max.x, p.x);
        bounds.max.y = std::max(bounds.max.y, p.y);
    }
    return bounds;
}

glm::vec2 normalizedToView(const glm::vec2& n) {
    constexpr float kLeft = -2.0f;
    constexpr float kRight = 2.0f;
    constexpr float kBottom = -1.5f;
    constexpr float kTop = 1.5f;
    const float x = kLeft + n.x * (kRight - kLeft);
    const float y = kTop - n.y * (kTop - kBottom);
    return {x, y};
}

StrokeBounds expandBoundsForMinSize(const std::vector<glm::vec2>& points, float minSize) {
    StrokeBounds bounds = boundsFromPoints(points);
    const glm::vec2 center = (bounds.min + bounds.max) * 0.5f;
    const float w = std::max(bounds.max.x - bounds.min.x, minSize);
    const float h = std::max(bounds.max.y - bounds.min.y, minSize);

    bounds.min = center - glm::vec2(w * 0.5f, h * 0.5f);
    bounds.max = center + glm::vec2(w * 0.5f, h * 0.5f);

    bounds.min = glm::clamp(bounds.min, glm::vec2(0.0f), glm::vec2(1.0f));
    bounds.max = glm::clamp(bounds.max, glm::vec2(0.0f), glm::vec2(1.0f));
    return bounds;
}
}  // namespace

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
    player = loadLevelFromFile("assets/levels/test2.json", textureManager, entityManager);

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
        // M1: capture stroke points between LMB down/up
        StrokeRecorder::instance()->poll();
        // Debug: log completed strokes count and size
        while (StrokeRecorder::instance()->hasCompletedStroke()) {
            auto s = StrokeRecorder::instance()->popCompletedStroke();
            if (s && !s->points.empty()) {
                std::cout << "[stroke] completed with " << s->points.size() << " points" << std::endl;
                // Submit to recognizer; log prediction only when one has been made
                Recognizer::instance()->submitStroke(s->points);
                if (auto pred = Recognizer::instance()->popNewPrediction()) {
                    std::cout << "[recognizer] label=" << pred->label
                              << ", conf=" << pred->confidence << std::endl;
                }
                {
                    const std::string textureName = "mossy_brick";
                    if (!textureManager->hasTexture(textureName)) {
                        textureManager->loadTexture(textureName, "assets/textures/" + textureName + ".png");
                    }
                    auto texPtr = textureManager->getTexture(textureName);
                    if (texPtr) {
                        constexpr float kMinStrokeSize = 0.03f;
                        const auto bounds = expandBoundsForMinSize(s->points, kMinStrokeSize);
                        const glm::vec2 vmin = normalizedToView(bounds.min);
                        const glm::vec2 vmax = normalizedToView(bounds.max);
                        const glm::vec2 size = glm::abs(vmax - vmin);
                        const glm::vec2 centerView = (vmin + vmax) * 0.5f;
                        const glm::vec2 centerWorld = centerView + glm::vec2(player->position.x, player->position.y);
                        entityManager->add<Platform>(PlatformType::stationary, texPtr,
                            glm::vec3(centerWorld, 0.0f), 0.0f, size, true);
                    }
                }
            }
        }

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
        constexpr float kViewScale = 2.0f;  // increase to zoom out
        glm::mat4 projection = glm::ortho(-2.0f * kViewScale, 2.0f * kViewScale,
                                          -1.5f * kViewScale, 1.5f * kViewScale,
                                          0.1f, 100.0f);

        // You can also use glm::perspective for 3D view, e.g.:
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.1f,
        // 100.0f);

        renderer->beginScene(view, projection);
        for (const auto &entity: entityManager->getEntities()) {
            // If this is our debug canvas overlay, anchor to screen and upload pixels on render
            // thread
            if (auto overlay = std::dynamic_pointer_cast<CanvasOverlay>(entity)) {
                // Place overlay slightly in front of camera (negative Z in view space)
                glm::vec3 screenAnchor(-1.7f, 1.0f, -1.0f);  // top-left-ish in our ortho view
                overlay->uploadToGpu();
            }

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
