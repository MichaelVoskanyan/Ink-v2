#pragma once
#include "gameObject.h"
#include <glm/glm.hpp>

enum class platformType_e { stationary, moving, falling, drawn };

class platform_t : public gameObject_t {
public:
    platformType_e type;
    bool isDrawn;

    platform_t(platformType_e type, const glm::vec3 &pos, float massValue, bool isDrawn = false);

    void update(float dt) override;
    void draw() override;
    bool hasCollision() const override {
        return true;
    }
};