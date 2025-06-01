#pragma once
#include "hitbox.h"
#include <glm/glm.hpp>
#include <renderer/renderer.h>
#include <renderer/textureManager.h>
#include <string>

class GameObject {
public:
    GameObject(const glm::vec2 &s = glm::vec2(1.0f, 1.0f),
               const glm::vec3 &p = glm::vec3(0.0f, 0.0f, 0.0f))
        : position(p), velocity(0.0f), scale(s), hitbox(s, p) {} // build hit-box once, with real data

    glm::vec3 position;
    glm::vec2 velocity;
    glm::vec2 scale;
    Hitbox hitbox;
    float mass = 0.0f;  // 0 ⇒ static / not affected by gravity

    std::shared_ptr<SceneObject> renderObject = nullptr;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // Override these in subclasses as needed
    virtual bool hasCollision() const {
        return false;
    }
    virtual void resolveCollision(GameObject *other) {
    }
    virtual bool shouldMoveOnCollision() const {
        return false;
    }

    bool affectedByGravity() const {
        return mass > 0.0f;
    }
    ~GameObject() = default;
};
