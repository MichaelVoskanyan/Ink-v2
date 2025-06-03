#ifndef INK_BUFFERS_H
#define INK_BUFFERS_H

#include <stl/ink_memory.h>

typedef unsigned int u32;
typedef float f32;
typedef double f64;

class IndexBuffer {
private:
    u32 m_rendererID;
    u32 m_count;

public:
    IndexBuffer(u32 *indices, u32 count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    u32 getCount() const;
};

class VertexBuffer {
private:
    u32 m_rendererID;
    u32 m_size;

public:
    VertexBuffer(f32 *vertices, u32 size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;
};

class VertexArray {
private:
    u32 m_rendererID;
    SharedPtr<VertexBuffer> m_vertexBuffer = nullptr;
    SharedPtr<IndexBuffer> m_indexBuffer = nullptr;

public:
    VertexArray(const SharedPtr<VertexBuffer> &vb, const SharedPtr<IndexBuffer> &ib);
    ~VertexArray();

    void bind() const;
    void unbind() const;

    u32 getIndexCount() const;
};

#endif