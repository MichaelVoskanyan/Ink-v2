#ifndef INK_BUFFERS_H
#define INK_BUFFERS_H

#include <memory>

using std::shared_ptr;

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
    shared_ptr<VertexBuffer> m_vertexBuffer;
    shared_ptr<IndexBuffer> m_indexBuffer;

public:
    VertexArray(const shared_ptr<VertexBuffer> &vb, const shared_ptr<IndexBuffer> &ib);
    ~VertexArray();

    void bind() const;
    void unbind() const;

    u32 getIndexCount() const;
};

#endif