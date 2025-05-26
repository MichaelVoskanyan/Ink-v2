#include "platform.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Static shared resources (all pointers start null; we initialize once because
// one character)
static bool s_ready = false;

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

platform_t::platform_t(platformType_e type, const glm::vec3 &startPos, float massValue,
                       const glm::vec2 &scale, bool isDrawn)
    : gameObject_t(scale, startPos), type(type), isDrawn(isDrawn) {
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

    // Initialize hitbox to match scaled size
    hitbox = hitbox_t(scale, position);

    // One‐time GL resource setup
    // if (!s_ready) {
    renderObject = std::make_shared<sceneObject_t>();
    renderObject->m_Mesh = std::make_shared<mesh_t>();
    renderObject->m_Mesh->m_vertexArray = std::make_shared<vertexArray_t>(
            std::make_shared<vertexBuffer_t>(s_verts, sizeof(s_verts)),
            std::make_shared<indexBuffer_t>(s_idx, sizeof(s_idx) / sizeof(s_idx[0])));
    renderObject->m_Mesh->m_Shader = std::make_shared<shader_t>("../source/shaders/platform.vs",
                                                                "../source/shaders/platform.fs");

    renderObject->m_Transform.m_position = position;
    renderObject->m_Transform.m_scale = glm::vec3(scale, 1.0);


    s_ready = true;
    // }
}

void platform_t::update(float dt) {
    // Update hitbox position to match platform position
    hitbox.updatePosition(glm::vec3(position));
    renderObject->m_Transform.m_position = position;
    renderObject->m_Transform.m_scale = glm::vec3(scale, 1.0);
}

void platform_t::draw() {
    return;
}