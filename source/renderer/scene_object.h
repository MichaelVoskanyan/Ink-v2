#ifndef INK_SCENEOBJECT_H
#define INK_SCENEOBJECT_H

#include "shader.h"
#include "buffers.h"
#include "texture.h"
#include "core/transform.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;


struct Mesh {
    shared_ptr<VertexArray> m_vertexArray;
    shared_ptr<Shader> m_shader;
    shared_ptr<Texture> m_texture;
};

struct SceneObject {
    shared_ptr<Mesh> m_mesh;
    Transform m_transform;
    bool m_screenSpace = false; // true to render in NDC/screen space
};


#endif  // INK_SCENEOBJECT_H
