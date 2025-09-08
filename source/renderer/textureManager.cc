// textureManager.cpp
#include "textureManager.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
// include your image‐loader (stb_image.h) and GL headers

std::shared_ptr<TextureManager> TextureManager::instance() {
    static auto inst = std::make_shared<TextureManager>();
    return inst;
}

bool TextureManager::loadTexture(const std::string &name, const std::string &filePath) {
    auto tex = std::make_shared<Texture>(filePath);
    if (!tex || tex->m_rendererID == 0) {
        std::cerr << "[TextureManager] Failed to load texture from " << filePath << "\n";
        return false;
    }

    std::cout << "[TextureManager] Successfully loaded texture: " << name << " (ID " << tex->m_rendererID << ")\n";
    sheetMap[name] = { tex, 0, 0 };
    return true;
}

// Create a GL texture intended for frequent CPU-side updates (RGBA8). Stores it
// under `name` so other systems can fetch it by key and render it.
bool TextureManager::createDynamicTexture(const std::string &name,
                                          int width,
                                          int height,
                                          const unsigned char *rgba) {
    auto tex = std::make_shared<Texture>(width, height, rgba, /*generateMipmaps*/ false);
    if (!tex || tex->m_rendererID == 0) {
        std::cerr << "[TextureManager] Failed to create dynamic texture: " << name << "\n";
        return false;
    }
    sheetMap[name] = { tex, 0, 0 };
    std::cout << "[TextureManager] Created dynamic texture: " << name << " (" << width << "x" << height
              << ", ID " << tex->m_rendererID << ")\n";
    return true;
}

// Replace the full contents of an existing dynamic texture with new RGBA pixels.
// Optionally regenerates mipmaps when `regenerateMipmaps` is true.
bool TextureManager::updateDynamicTexture(const std::string &name,
                                          const unsigned char *rgba,
                                          bool regenerateMipmaps) {
    auto it = sheetMap.find(name);
    if (it == sheetMap.end() || !it->second.texture) {
        std::cerr << "[TextureManager] updateDynamicTexture: missing texture '" << name << "'\n";
        return false;
    }
    it->second.texture->update(rgba, regenerateMipmaps);
    return true;
}

shared_ptr<Texture> TextureManager::getTexture(const string &name) const {
    auto it = sheetMap.find(name);
    if (it == sheetMap.end()) {
        std::cerr << "[getTexture] Texture '" << name << "' not found in sheetMap!\n";
        return nullptr;
    }
    return it != sheetMap.end() ? it->second.texture : nullptr;
}

bool TextureManager::hasTexture(const string &fileName) {
    return sheetMap.find(fileName) != sheetMap.end();
}

bool TextureManager::loadSpriteSheet(const string &sheetName, const string &filePath, int sW, int sH) {
    if (!loadTexture(sheetName, filePath))
        return false;

    // overwrite the sheet entry with correct sprite dims
    auto &sh = sheetMap[sheetName];
    sh.spriteWidth = sW;
    sh.spriteHeight = sH;
    return true;
}

bool TextureManager::registerSprite(const string &spriteName, const string &sheetName,
                                    int frameIndex) {
    auto it = sheetMap.find(sheetName);
    if (it == sheetMap.end()) {
        cerr << "No sheet loaded as \"" << sheetName << "\"\n";
        return false;
    }
    const auto &sh = it->second;
    spriteMap[spriteName] = {sh.texture, sh.spriteWidth, sh.spriteHeight, frameIndex};
    return true;
}

const SpriteInfo *TextureManager::getSpriteInfo(const string &spriteName) const {
    auto it = spriteMap.find(spriteName);
    return it != spriteMap.end() ? &it->second : nullptr;
}
