#pragma once

#include <glm/glm.hpp>
// Ensure GLFW does not include OpenGL headers (we use glad)
#define GLFW_INCLUDE_NONE
#include "gameObject.h"
#include "renderer/renderer.h"
#include <GLFW/glfw3.h>

// drawMode_e lets us easily add new brush types later
enum class drawMode_e { none, platform, weapon, projectile };

class character_t : public gameObject_t {
public:
    // speed scalar for planar movement
    float speed = 1.0f;
    shared_ptr<sceneObject_t> renderObject;

    drawMode_e drawMode = drawMode_e::none;

    // Constructor: initialize position and speed, optional mass
    character_t(const glm::vec3 &startPos, float speedValue, float massValue,
                const glm::vec2 &scale);

    // Input handling
    void handleKeyInput();
    void handleMouseInput();

    // Overrides
    void update(float dt) override;
    void draw() override {
        return;
    }
    void resolveCollision(gameObject_t *other);
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