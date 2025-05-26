#include "character.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using std::make_shared;

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

character_t::character_t(const glm::vec3 &startPos, float speedValue, float massValue,
                         const glm::vec2 &scale)
    : gameObject_t(scale, startPos), speed(speedValue) {
    // Initialize base class members
    this->scale = scale;
    mass = massValue;
    position = startPos;
    velocity = glm::vec2(0.0f);

    // Initialize hitbox to match scaled size
    hitbox = hitbox_t(scale, position);

    // One‐time GL resource setup
    if (!s_ready) {
        renderObject = make_shared<sceneObject_t>();
        renderObject->m_Mesh = make_shared<mesh_t>();
        renderObject->m_Mesh->m_vertexArray = make_shared<vertexArray_t>(
                make_shared<vertexBuffer_t>(s_verts, sizeof(s_verts)),
                make_shared<indexBuffer_t>(s_idx, sizeof(s_idx) / sizeof(s_idx[0])));
        renderObject->m_Mesh->m_Shader =
                make_shared<shader_t>("../source/shaders/char.vs", "../source/shaders/char.fs");

        s_ready = true;
    }
}

void character_t::handleKeyInput() {
    GLFWwindow *win = glfwGetCurrentContext();
    glm::vec2 dir(0.0f);
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        dir.x -= 1.0f;
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        dir.x += 1.0f;
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS && !m_isJumping) {
        velocity.y += 0.1f;
        m_isJumping = true;
    }
    if (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_RELEASE && m_isJumping) {
        m_isJumping = false;
    }

    // Apply horizontal movement with reduced speed
    velocity.x = dir.x * (speed * 0.2f);  // Reduce the speed to 20% of the original
}

void character_t::handleMouseInput() {
    GLFWwindow *win = glfwGetCurrentContext();
    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        drawMode = static_cast<drawMode_e>((int(drawMode) + 1) % 4);
    }
}

void character_t::resolveCollision(gameObject_t *other) {
    std::cout << "[character] Checking collision with platform" << std::endl;
    std::cout << "  Character pos: (" << position.x << ", " << position.y << ")" << std::endl;
    std::cout << "  Character velocity: (" << velocity.x << ", " << velocity.y << ")" << std::endl;

    if (hitbox.intersects(other->hitbox)) {
        if (other->hasCollision() && this->shouldMoveOnCollision()) {
            std::cout << "[character] Platform has collision and character can move" << std::endl;
            // Move the character out of the other object
            glm::vec2 resolution = hitbox.getCollisionResolution(other->hitbox);
            position += glm::vec3(resolution, 0.0f);
            renderObject->m_Transform.m_position = position;

            // Kill velocity in the direction of collision
            if (resolution.x != 0.0f) {
                velocity.x = 0.0f;  // Kill horizontal velocity if colliding horizontally
                std::cout << "[character] Killed horizontal velocity" << std::endl;
            }
            if (resolution.y != 0.0f) {
                velocity.y = 0.0f;  // Kill vertical velocity if colliding vertically
                if (resolution.y > 0.0f) {
                    m_isJumping = false;  // Reset jumping state when landing
                }
                std::cout << "[character] Killed vertical velocity and reset jump" << std::endl;
            }
        }
    }
}

void character_t::update(float dt) {
    handleKeyInput();
    handleMouseInput();
    // Optional gravity
    if (affectedByGravity()) {
        velocity.y -= 1.0f * dt * mass;  // Multiply by mass instead of dividing
    }
    // Move in X/Y, keep Z unchanged
    position += glm::vec3(velocity * speed * dt, 0.0f);

    // Update hitbox position to match character position
    hitbox.updatePosition(glm::vec3(position));

    renderObject->m_Transform.m_position = position;
    renderObject->m_Transform.m_scale = glm::vec3(scale, 1.0);
}
