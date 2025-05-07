#include "buffers.h"

#include <glad/glad.h>

indexBuffer_t::indexBuffer_t(u32 *indices, u32 count) : count(count) {
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
}

indexBuffer_t::~indexBuffer_t() {
    glDeleteBuffers(1, &rendererID);
}

void indexBuffer_t::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}

void indexBuffer_t::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

u32 indexBuffer_t::getCount() const {
    return count;
}

vertexBuffer_t::vertexBuffer_t(f32 *vertices, u32 size) : rendererID(0), size(size) {
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

vertexBuffer_t::~vertexBuffer_t() {
    glDeleteBuffers(1, &rendererID);
}

void vertexBuffer_t::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}

void vertexBuffer_t::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

vertexArray_t::vertexArray_t() : rendererID(0), vertexBuffer(nullptr), indexBuffer(nullptr) {
    glGenVertexArrays(1, &rendererID);
    glBindVertexArray(rendererID);
}

vertexArray_t::~vertexArray_t() {
    glDeleteVertexArrays(1, &rendererID);
}

void vertexArray_t::attachVertexBuffer(vertexBuffer_t *vertBuf) {
    glBindVertexArray(rendererID);
    vertBuf->bind();
    vertexBuffer = vertBuf;
}

void vertexArray_t::attachIndexBuffer(indexBuffer_t *indBuf) {
    glBindVertexArray(rendererID);
    indBuf->bind();
    indexBuffer = indBuf;
}

void vertexArray_t::bind() const {
    glBindVertexArray(rendererID);
}

void vertexArray_t::unbind() const {
    glBindVertexArray(0);
}

u32 vertexArray_t::getIndexCount() const {
    return indexBuffer->getCount();
}
