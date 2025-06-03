#include "platform.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include <stl/ink_memory.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Quad vertex data: positions only (we stride through 8 floats, but only read
// 3) In case we want to do more complex shaders later, probably can be
// simplified
static unsigned int s_idx[] = {0, 1, 2, 0, 2, 3};

Platform::Platform(PlatformType type,
                   SharedPtr<Texture> texture,
                   const glm::vec3 &startPos,
                   float massValue,
                   const glm::vec2 &scale,
                   bool isDrawn)
    : GameObject(scale, startPos), type(type), isDrawn(isDrawn) {
    this->scale = scale;
    position = startPos;
    mass = 0.f;

    std::cout << "[platform] Created:\n";
    std::cout << "  Type: " << static_cast<int>(type) << "\n";
    std::cout << "  Position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
    std::cout << "  Scale: (" << scale.x << ", " << scale.y << ")\n";
    std::cout << "  Mass: " << mass << "\n";

    renderObject = makeShared<SceneObject>();
    renderObject->m_mesh = makeShared<Mesh>();
    assert(texture && "Texture pointer is null!");
    assert(renderObject && "renderObject is null!");
    assert(renderObject->m_mesh && "renderObject->m_mesh is null!");
    renderObject->m_mesh->m_texture = texture;

    float repeatX = scale.x;
    float repeatY = scale.y;
    float verts[] = {
        -0.5f, -0.5f, 0.0f, 0, 0, 1, 0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0, 0, 1, 0.0f, repeatY,
         0.5f,  0.5f, 0.0f, 0, 0, 1, repeatX, repeatY,
         0.5f, -0.5f, 0.0f, 0, 0, 1, repeatX, 0.0f
    };

    static unsigned int s_idx[] = {0, 1, 2, 0, 2, 3};

    renderObject->m_mesh->m_vertexArray = makeShared<VertexArray>(
        makeShared<VertexBuffer>(verts, sizeof(verts)),
        makeShared<IndexBuffer>(s_idx, sizeof(s_idx) / sizeof(s_idx[0])));
    renderObject->m_mesh->m_shader = makeShared<Shader>("platform.vs", "platform.fs");

    renderObject->m_transform.m_position = position;
    renderObject->m_transform.m_scale = glm::vec3(scale, 1.0f);
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