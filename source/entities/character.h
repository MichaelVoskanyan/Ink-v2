#pragma once
#include "gameObject.h"
#include <glm/glm.hpp>

enum class drawMode_e {
    none,
    platform,
    weapon,
    projectile
}; // adding an enum for drawMode_e makes it easy to add different brush types later

class character_t : public gameObject_t {
public:
    glm::vec2 velocity;
    glm::vec3 position;
    float mass = 1.0f;
    drawMode_e drawMode = drawMode_e::none;

    void handleKeyInput();
    void handleMouseInput();
    void update(float dt) override;
    void draw() override;
    bool hasCollision() const override { // maybe should make this not const?
        return true;
    }
};