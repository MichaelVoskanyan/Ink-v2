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
    renderObject->m_mesh = std::make_shared<Mesh>();
    renderObject->m_mesh->m_texture = s_textureManager->getTexture("mossy_brick");
    float repeatX = scale.x;
    float repeatY = scale.y;
    
    // Vertex format: position (3), normal (3), uv (2) = 8 floats per vertex
    float verts[] = {
        // pos               // normal     // uv (scaled by repeat)
        -0.5f, -0.5f, 0.0f,   0, 0, 1,      0.0f,      0.0f,
        -0.5f,  0.5f, 0.0f,   0, 0, 1,      0.0f,      repeatY,
         0.5f,  0.5f, 0.0f,   0, 0, 1,      repeatX,   repeatY,
         0.5f, -0.5f, 0.0f,   0, 0, 1,      repeatX,   0.0f
    };
    
    renderObject->m_mesh->m_vertexArray = std::make_shared<VertexArray>(
        std::make_shared<VertexBuffer>(verts, sizeof(verts)),
        std::make_shared<IndexBuffer>(s_idx, sizeof(s_idx) / sizeof(s_idx[0])));
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