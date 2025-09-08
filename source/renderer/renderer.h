#ifndef INK_RENDERER_H
#define INK_RENDERER_H

#include "textureManager.h"

#include "scene_object.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

class Renderer {
    static Renderer *s_instance;

    Renderer() = default;
    ~Renderer() = default;

    vector<shared_ptr<SceneObject>> m_renderQueue;
    glm::mat4 m_viewMat;
    glm::mat4 m_projMat;

public:
    static Renderer *getInstance() {
        if (s_instance == nullptr) {
            s_instance = new Renderer();
        }
        return s_instance;
    }

    void beginScene(const glm::mat4 &viewMat, const glm::mat4 &projMat) {
        m_viewMat = viewMat;
        m_projMat = projMat;
    }

    void submit(const std::shared_ptr<SceneObject> &object) {
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

            if (obj->m_mesh->m_texture) {
                obj->m_mesh->m_texture->bind();
            }

            obj->m_mesh->m_shader->setMat4("u_viewMat", m_viewMat);
            obj->m_mesh->m_shader->setMat4("u_projMat", m_projMat);

            obj->m_mesh->m_shader->setVec3("u_position", obj->m_transform.m_position);
            obj->m_mesh->m_shader->setVec3("u_scale", obj->m_transform.m_scale);
            obj->m_mesh->m_shader->setInt("u_screenSpace", obj->m_screenSpace ? 1 : 0);

            glDrawElements(GL_TRIANGLES, obj->m_mesh->m_vertexArray->getIndexCount(),
                           GL_UNSIGNED_INT, nullptr);
        }
    }
};

#endif
