#pragma once
#include "gameObject.h"
#include "renderer/renderer.h"
#include "renderer/textureManager.h"
#include <glm/glm.hpp>

enum class PlatformType { stationary, moving, falling, drawn };

class Platform : public GameObject {
public:
    PlatformType type;
    bool isDrawn;

    Platform(PlatformType type,
        std::shared_ptr<Texture> texture,
        const glm::vec3 &startPos,
        float massValue,
        const glm::vec2 &scale,
        bool isDrawn);

    void update(float dt) override;
    void draw() override;
    bool hasCollision() const override {
        // Only stationary and moving platforms have collision
        return type == PlatformType::stationary || type == PlatformType::moving;
    }
};