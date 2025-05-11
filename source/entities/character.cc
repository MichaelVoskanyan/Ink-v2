#include "character.h"
#include "renderer/shader.h"
#include "renderer/buffers.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

// Static shared resources (all pointers start null; we initialize once because one character)
static shader_t *s_shader = nullptr;
static vertexArray_t *s_vao = nullptr;
static vertexBuffer_t *s_vbo = nullptr;
static indexBuffer_t *s_ibo = nullptr;
static bool s_ready = false;

// Quad vertex data: positions only (we stride through 8 floats, but only read 3)
// In case we want to do more complex shaders later, probably can be simplified
static float s_verts[] = {-0.5f, -0.5f, 0.0f, /* skip 5 floats */ 0,
                          0,     1,     0,    0,
                          -0.5f, 0.5f,  0.0f, 0,
                          0,     1,     0,    1,
                          0.5f,  0.5f,  0.0f, 0,
                          0,     1,     1,    1,
                          0.5f,  -0.5f, 0.0f, 0,
                          0,     1,     1,    0};
static unsigned int s_idx[] = {0, 1, 2, 0, 2, 3};

character_t::character_t(const glm::vec3 &startPos, float speedValue) : speed(speedValue) {
    // One‐time GL resource setup
    if(!s_ready) {
        // 1. Compile & bind our minimal shader
        s_shader = new shader_t("char.vs", "char.fs");
        s_shader->bind();

        // 2. Allocate VAO/VBO/IBO now that GLAD + context are ready
        s_vao = new vertexArray_t();
        s_vbo = new vertexBuffer_t(s_verts, sizeof(s_verts));
        s_ibo = new indexBuffer_t(s_idx, sizeof(s_idx) / sizeof(s_idx[0]));

        // 3. Attach & bind
        s_vao->attachVertexBuffer(s_vbo);
        s_vao->attachIndexBuffer(s_ibo);
        s_vao->bind();

        // 4. Define position attribute only (location = 0)
        constexpr GLsizei stride = 8 * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            /* index */ 0,
            /* size */ 3,
            /* type */ GL_FLOAT,
            /* norm */ GL_FALSE,
            /* stride */ stride,
            /* offset */ (void *)0);

        s_ready = true;
    }

    // Initialize instance state
    position = startPos;
    velocity = glm::vec2(0.0f);
    mass = 1.0f;
}

void character_t::handleKeyInput() {
    GLFWwindow *win = glfwGetCurrentContext();
    glm::vec2 dir(0.0f);
    if(glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
        dir.y += 1.0f;
    if(glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
        dir.y -= 1.0f;
    if(glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
        dir.x -= 1.0f;
    if(glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
        dir.x += 1.0f;
    velocity = (glm::length(dir) > 0.0f) ? glm::normalize(dir) : glm::vec2(0.0f);
}

void character_t::handleMouseInput() {
    GLFWwindow *win = glfwGetCurrentContext();
    if(glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        drawMode = static_cast<drawMode_e>((int(drawMode) + 1) % 4);
    }
}

void character_t::update(float dt) {
    // Optional gravity
    if(affectedByGravity()) {
        velocity.y -= 9.8f * dt / mass;
    }
    // Move in X/Y, keep Z unchanged
    position += glm::vec3(velocity * speed * dt, 0.0f);

    handleKeyInput();
    handleMouseInput();
}

void character_t::draw() {
    s_shader->bind();

    // Model = translate only (no scale/rotation)
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    s_shader->setMat4("u_Model", model);

    // Draw the quad
    s_vao->bind();
    s_ibo->bind();
    glDrawElements(GL_TRIANGLES, s_ibo->getCount(), GL_UNSIGNED_INT, nullptr);
}
