#include "platform.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Quad vertex data: positions only (we stride through 8 floats, but only read
// 3) In case we want to do more complex shaders later, probably can be
// simplified
static float s_verts[] = {-0.5f, -0.5f, 0.0f, /* skip 5 floats */ 0,
                          0,     1,     0,    0,
                          -0.5f, 0.5f,  0.0f, 0,
                          0,     1,     0,    1,
                          0.5f,  0.5f,  0.0f, 0,
                          0,     1,     1,    1,
                          0.5f,  -0.5f, 0.0f, 0,
                          0,     1,     1,    0};
static unsigned int s_idx[] = {0, 1, 2, 0, 2, 3};

Platform::Platform(PlatformType type, const glm::vec3 &startPos, float massValue,
                   const glm::vec2 &scale, bool isDrawn)
    : GameObject(scale, startPos), type(type), isDrawn(isDrawn) {
    // Initialize base class members
    this->scale = scale;
    position = startPos;
    mass = 0.f;

    std::cout << "[platform] Created:" << std::endl;
    std::cout << "  Type: " << (int) type << std::endl;
    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")"
              << std::endl;
    std::cout << "  Scale: (" << scale.x << ", " << scale.y << ")" << std::endl;
    std::cout << "  Mass: " << mass << std::endl;

    // One‐time GL resource setup
    renderObject = std::make_shared<SceneObject>();
    renderObject->m_textureID = s_textureManager->getTexture("mossy_brick");
    renderObject->m_mesh = std::make_shared<Mesh>();
    renderObject->m_mesh->m_vertexArray = std::make_shared<VertexArray>(
            std::make_shared<VertexBuffer>(s_verts, (u32) sizeof(s_verts)),
            std::make_shared<IndexBuffer>(s_idx, (u32) (sizeof(s_idx) / sizeof(s_idx[0]))));
    renderObject->m_mesh->m_shader = std::make_shared<Shader>("platform.vs", "platform.fs");

    renderObject->m_transform.m_position = position;
    renderObject->m_transform.m_scale = glm::vec3(scale, 1.0);
}

void Platform::update(float dt) {
    // Update hitbox position to match platform position
    hitbox.updatePosition(glm::vec3(position));

    renderObject->m_transform.m_position = position;
    renderObject->m_transform.m_scale = glm::vec3(scale, 1.0);
}

void Platform::draw() {
    return;
}