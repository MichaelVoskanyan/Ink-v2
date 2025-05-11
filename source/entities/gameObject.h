#pragma once
#include <string>
#include <glm/glm.hpp>

class gameObject_t {
public:
    glm::vec3 position;
    glm::vec2 velocity;
    float mass = 0.0f; // 0 = static or not affected by gravity

    virtual void update(float dt) = 0;
    virtual void draw() = 0;
    virtual bool hasCollision() const {
        return false;
    }
    bool affectedByGravity() const {
        return mass > 0.0f;
    }

    virtual ~gameObject_t() = default;
};