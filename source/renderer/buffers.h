#ifndef INK_BUFFERS_H
#define INK_BUFFERS_H

typedef unsigned int u32;
typedef float f32;
typedef double f64;

class indexBuffer_t {
private:
    u32 rendererID;
    u32 count;

public:
    indexBuffer_t(u32 *indices, u32 count);
    ~indexBuffer_t();

    void bind() const;
    void unbind() const;

    u32 getCount() const;
};

class vertexBuffer_t {
private:
    u32 rendererID;
    u32 size;

public:
    vertexBuffer_t(f32 *vertices, u32 size);
    ~vertexBuffer_t();

    void bind() const;
    void unbind() const;
};

class vertexArray_t {
private:
    u32 rendererID;
    vertexBuffer_t *vertexBuffer;
    indexBuffer_t *indexBuffer;

public:
    vertexArray_t();
    ~vertexArray_t();

    void attachVertexBuffer(vertexBuffer_t *vertBuf);
    void attachIndexBuffer(indexBuffer_t *indBuf);

    void bind() const;
    void unbind() const;

    u32 getIndexCount() const;
};

#endif