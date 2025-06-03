#ifndef INK_SCENEOBJECT_H
#define INK_SCENEOBJECT_H

#include "shader.h"
#include "buffers.h"
#include "texture.h"
#include "core/transform.h"

#include <stl/ink_memory.h>
#include <vector>

using std::vector;


struct Mesh {
    SharedPtr<VertexArray> m_vertexArray;
    SharedPtr<Shader> m_shader;
    SharedPtr<Texture> m_texture;
};

struct SceneObject {
    SharedPtr<Mesh> m_mesh;
    Transform m_transform;
};


#endif  // INK_SCENEOBJECT_H