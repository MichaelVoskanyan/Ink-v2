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
    bool m_hasMipmaps = false;

    // Load from file (RGBA), generate mipmaps by default
    Texture(std::string path) {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 4);
        if (data) {
            glGenTextures(1, &m_rendererID);
            glBindTexture(GL_TEXTURE_2D, m_rendererID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                         data);
            glGenerateMipmap(GL_TEXTURE_2D);
            m_hasMipmaps = true;

            // Reasonable defaults for file textures
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << path << '\n';
            m_rendererID = 0;
        }
    }

    // Create a dynamic RGBA texture (CPU-updatable)
    Texture(int width, int height, const unsigned char *rgba = nullptr, bool generateMipmaps = false) {
        m_width = width;
        m_height = height;
        m_channels = 4;
        m_hasMipmaps = generateMipmaps;

        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        if (generateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        // Defaults for HUD/overlay textures: clamp, linear, no mipmaps by default
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Update the full texture from CPU memory (RGBA8)
    void update(const unsigned char *rgba, bool regenerateMipmaps = false) {
        if (!m_rendererID) return;
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
        if (regenerateMipmaps) {
            glGenerateMipmap(GL_TEXTURE_2D);
            m_hasMipmaps = true;
        }
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, m_rendererID);

        // Keep filters consistent with mipmap availability
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_hasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
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
