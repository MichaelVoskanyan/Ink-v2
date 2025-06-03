#include "buffers.h"

#include <glad/glad.h>

IndexBuffer::IndexBuffer(u32 *indices, u32 count) : m_count(count) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_rendererID);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 IndexBuffer::getCount() const {
    return m_count;
}

VertexBuffer::VertexBuffer(f32 *vertices, u32 size) : m_rendererID(0), m_size(size) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_rendererID);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray::VertexArray(const SharedPtr<VertexBuffer> &vb, const SharedPtr<IndexBuffer> &ib)
    : m_rendererID(0), m_vertexBuffer(vb), m_indexBuffer(ib) {
    glGenVertexArrays(1, &m_rendererID);
    glBindVertexArray(m_rendererID);

    m_vertexBuffer->bind();
    m_indexBuffer->bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *) (sizeof(f32) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void *) (sizeof(f32) * 6));
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::bind() const {
    glBindVertexArray(m_rendererID);
    m_vertexBuffer->bind();
    m_indexBuffer->bind();
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

u32 VertexArray::getIndexCount() const {
    return m_indexBuffer->getCount();
}
