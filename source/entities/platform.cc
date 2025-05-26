#include "platform.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Static shared resources (all pointers start null; we initialize once because
// one character)
static shader_t *s_shader = nullptr;
static vertexArray_t *s_vao = nullptr;
static vertexBuffer_t *s_vbo = nullptr;
static indexBuffer_t *s_ibo = nullptr;
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

platform_t::platform_t(platformType_e type, glm::vec3 &startPos,
                       float massValue, const glm::vec2 &scale, bool isDrawn)
    : gameObject_t(scale, startPos), type(type), isDrawn(isDrawn) {
  // Initialize base class members
  this->scale = scale;
  position = startPos;
  mass = 0.f;

  std::cout << "[platform] Created:" << std::endl;
  std::cout << "  Type: " << (int)type << std::endl;
  std::cout << "  Position: (" << position.x << ", " << position.y << ", "
            << position.z << ")" << std::endl;
  std::cout << "  Scale: (" << scale.x << ", " << scale.y << ")" << std::endl;
  std::cout << "  Mass: " << mass << std::endl;

  // One‐time GL resource setup
  if (!s_ready) {
    // 1. Compile & bind our minimal shader
    s_shader = new shader_t("platform.vs", "platform.fs");
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
}

void platform_t::update(float dt) {
  // Update hitbox position to match platform position
  //   hitbox.updatePosition(glm::vec3(position));
}

void platform_t::draw() {
  s_shader->bind();

  // Model = translate and scale
  glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
  model =
      glm::scale(model, glm::vec3(scale, 1.0f)); // Convert vec2 scale to vec3
  s_shader->setMat4("u_Model", model);

  // Draw the quad65
  s_vao->bind();
  s_ibo->bind();
  glDrawElements(GL_TRIANGLES, s_ibo->getCount(), GL_UNSIGNED_INT, nullptr);
}