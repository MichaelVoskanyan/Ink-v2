#pragma once
#include <string>
#include <glm/glm.hpp>
#include "hitbox.h"

class gameObject_t {
public:
    glm::vec3 position;
    glm::vec2 velocity;
    glm::vec2 scale;
    hitbox_t hitbox;
    float mass = 0.0f; // 0 = static or not affected by gravity

    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual bool hasCollision() const {
        return false;
    }
    bool affectedByGravity() const {
        return mass > 0.0f;
    }
    virtual bool shouldMoveOnCollision() const {
        return false; // By default, objects don't move during collision resolution
    }

    virtual ~gameObject_t() = default;
};