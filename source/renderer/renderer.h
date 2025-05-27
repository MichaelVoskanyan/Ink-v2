#ifndef INK_RENDERER_H
#define INK_RENDERER_H

#include "shader.h"
#include "buffers.h"
#include "textureManager.h"
#include "../core/transform.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

struct mesh_t {
    shared_ptr<vertexArray_t> m_vertexArray;
    shared_ptr<shader_t> m_shader;
};

struct sceneObject_t {
    shared_ptr<mesh_t> m_mesh;
    uint32_t m_textureID;
    transform_t m_transform;
};

class renderer_t {
    static renderer_t *s_instance;

    renderer_t() = default;
    ~renderer_t() = default;

    vector<shared_ptr<sceneObject_t>> m_renderQueue;
    glm::mat4 m_viewMat;
    glm::mat4 m_projMat;

public:
    static renderer_t *getInstance() {
        if (s_instance == nullptr) {
            s_instance = new renderer_t();
        }
        return s_instance;
    }

    void beginScene(const glm::mat4 &viewMat, const glm::mat4 &projMat) {
        m_viewMat = viewMat;
        m_projMat = projMat;
    }

    void submit(const std::shared_ptr<sceneObject_t> &object) {
        m_renderQueue.push_back(object);
    }

    void clearQueue() {
        m_renderQueue.clear();
    }

    void endScene() {
        glClear(GL_COLOR_BUFFER_BIT);

        for (const auto &obj: m_renderQueue) {
            obj->m_mesh->m_vertexArray->bind();
            obj->m_mesh->m_shader->bind();

            obj->m_mesh->m_shader->setMat4("u_viewMat", m_viewMat);
            obj->m_mesh->m_shader->setMat4("u_projMat", m_projMat);

            obj->m_mesh->m_shader->setVec3("u_position", obj->m_transform.m_position);
            obj->m_mesh->m_shader->setVec3("u_scale", obj->m_transform.m_scale);

            glDrawElements(GL_TRIANGLES, obj->m_mesh->m_vertexArray->getIndexCount(),
                           GL_UNSIGNED_INT, nullptr);
        }
    }
};

#endif