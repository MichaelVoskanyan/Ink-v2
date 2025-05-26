#ifndef INK_RENDERER_H
#define INK_RENDERER_H

#include "shader.h"
#include "buffers.h"
#include "../core/transform.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

struct mesh_t {
    shared_ptr<vertexArray_t> m_vertex_array;
    shared_ptr<shader_t> m_shader;
};

struct sceneObject_t {
    shared_ptr<mesh_t> m_mesh;
    transform_t m_transform;
};

class renderer_t {
    vector<shared_ptr<sceneObject_t>> m_render_queue;
    glm::mat4 m_view_mat;
    glm::mat4 m_proj_mat;

public:
    void beginScene(const glm::mat4 &view_mat, const glm::mat4 &proj_mat) {
        m_view_mat = view_mat;
        m_proj_mat = proj_mat;
    }

    void submit(const std::shared_ptr<sceneObject_t> &object) {
        m_render_queue.push_back(object);
    }

    void clearQueue() {
        m_render_queue.clear();
    }

    void endScene() {
        glClear(GL_COLOR_BUFFER_BIT);

        for (const auto &obj: m_render_queue) {
            obj->m_mesh->m_vertex_array->bind();
            obj->m_mesh->m_shader->bind();

            obj->m_mesh->m_shader->setMat4("u_view_mat", m_view_mat);
            obj->m_mesh->m_shader->setMat4("u_proj_mat", m_proj_mat);

            obj->m_mesh->m_shader->setVec3("u_position", obj->m_transform.m_position);
            obj->m_mesh->m_shader->setVec3("u_scale", obj->m_transform.m_scale);

            glDrawElements(GL_TRIANGLES, obj->m_mesh->m_vertex_array->getIndexCount(),
                           GL_UNSIGNED_INT, nullptr);
        }
    }
};

#endif