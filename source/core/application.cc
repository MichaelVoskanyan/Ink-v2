#include "application.h"
#include <renderer/buffers.h>
#include <renderer/shader.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

application_t *application_t::s_instance = nullptr;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

application_t::application_t() {
    if(!glfwInit())
        throw new std::runtime_error("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if(!window)
        throw new std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw new std::runtime_error("Failed to initialize GLAD");

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glClearColor(0.589f, 0.443f, 0.09f, 1.f);
}

application_t *application_t::getInstance() {
    if(nullptr == s_instance) {
        s_instance = new application_t();
    }
    return s_instance;
}

void application_t::run() {
    float vertices[] = {-0.5f, -0.5f, 0.0f, 0.f, 0.f, 0.f, 0.f, 0.f, -0.5f, 0.5f,  0.0f, 0.f, 0.f, 0.f, 0.f, 1.f,
                        0.5f,  0.5f,  0.0f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.5f,  -0.5f, 0.0f, 0.f, 0.f, 0.f, 1.f, 0.f};

    unsigned int indices[] = {0, 1, 2, 0, 2, 3};
    shader_t s("/source/shaders/tri.vs", "/source/shaders/tri.fs");
    s.bind();

    vertexArray_t va;
    vertexBuffer_t *vb = new vertexBuffer_t(vertices, sizeof(vertices));
    indexBuffer_t *ib = new indexBuffer_t(indices, sizeof(indices) / sizeof(indices[0]));

    va.attachVertexBuffer(vb);
    va.attachIndexBuffer(ib);

    va.bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(3 * sizeof(f32)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *)(6 * sizeof(f32)));

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DRAW STUFF
        s.bind();
        va.bind();
        vb->bind();
        ib->bind();
        glDrawElements(GL_TRIANGLES, ib->getCount(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
