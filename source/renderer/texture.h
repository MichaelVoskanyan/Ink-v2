#ifndef INK_TEXTURE_H
#define INK_TEXTURE_H

#include <stb_image.h>

#include <glad/glad.h>
#include <string>
#include <cstdint>
#include <iostream>

struct Texture {
    uint32_t m_rendererID;
    int m_width, m_height, m_channels;

    Texture(std::string path) {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 4);
        if (data) {
            glGenTextures(1, &m_rendererID);
            glBindTexture(GL_TEXTURE_2D, m_rendererID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << path << '\n';
            m_rendererID = 0;
        }
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ~Texture() {
        if (m_rendererID) {
            glDeleteTextures(1, &m_rendererID);
        }
    }
};

#endif