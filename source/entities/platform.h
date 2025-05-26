#pragma once
#include "gameObject.h"
#include "renderer/renderer.h"
#include "hitbox.h"
#include <glm/glm.hpp>

enum class platformType_e { stationary, moving, falling, drawn };

class platform_t : public gameObject_t {
public:
    platformType_e type;
    bool isDrawn;
    hitbox_t hitbox;
    shared_ptr<sceneObject_t> renderObject;

    platform_t(platformType_e type, const glm::vec3 &pos, float massValue, const glm::vec2 &scale,
               bool isDrawn);

    void update(float dt) override;
    void draw() override;
    bool hasCollision() const override {
        // Only stationary and moving platforms have collision
        return type == platformType_e::stationary || type == platformType_e::moving;
    }
};