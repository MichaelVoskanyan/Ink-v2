#pragma once

#include <glm/glm.hpp>
// Ensure GLFW does not include OpenGL headers (we use glad)
#define GLFW_INCLUDE_NONE
#include "gameObject.h"
#include "renderer/renderer.h"
#include <GLFW/glfw3.h>

// drawMode_e lets us easily add new brush types later
enum class DrawMode { none, platform, weapon, projectile };

class Character : public GameObject {
public:
    // speed scalar for planar movement
    float speed = 1.0f;

    DrawMode drawMode = DrawMode::none;

    // Constructor: initialize position and speed, optional mass
    Character(const glm::vec3 &startPos, float speedValue, float massValue, const glm::vec2 &scale);

    // Input handling
    void handleKeyInput();
    void handleMouseInput();

    // Overrides
    void update(float dt) override;
    void draw() override {
        return;
    }
    void resolveCollision(GameObject *other);
    bool hasCollision() const override {
        return true;
    }
    bool shouldMoveOnCollision() const override {
        return true;  // Character should move during collision resolution
    }
    bool isJumping() const {
        return m_isJumping;
    }

private:
    bool m_isJumping = false;
};