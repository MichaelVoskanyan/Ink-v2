#include "buffers.h"

#include <glad/glad.h>

indexBuffer_t::indexBuffer_t(u32 *indices, u32 count) : m_count(count) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
}

indexBuffer_t::~indexBuffer_t() {
    glDeleteBuffers(1, &m_rendererID);
}

void indexBuffer_t::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void indexBuffer_t::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 indexBuffer_t::getCount() const {
    return m_count;
}

vertexBuffer_t::vertexBuffer_t(f32 *vertices, u32 size) : m_rendererID(0), m_size(size) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

vertexBuffer_t::~vertexBuffer_t() {
    glDeleteBuffers(1, &m_rendererID);
}

void vertexBuffer_t::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void vertexBuffer_t::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

vertexArray_t::vertexArray_t(const shared_ptr<vertexBuffer_t> &vb,
                             const shared_ptr<indexBuffer_t> &ib)
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

vertexArray_t::~vertexArray_t() {
    glDeleteVertexArrays(1, &m_rendererID);
}

void vertexArray_t::bind() const {
    glBindVertexArray(m_rendererID);
    m_vertexBuffer->bind();
    m_indexBuffer->bind();
}

void vertexArray_t::unbind() const {
    glBindVertexArray(0);
}

u32 vertexArray_t::getIndexCount() const {
    return m_indexBuffer->getCount();
}
