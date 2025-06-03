// textureManager.cpp
#include "textureManager.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
// include your image‐loader (stb_image.h) and GL headers

SharedPtr<TextureManager> TextureManager::instance() {
    static auto inst = makeShared<TextureManager>();
    return inst;
}

bool TextureManager::loadTexture(const std::string &name, const std::string &filePath) {
    auto tex = makeShared<Texture>(filePath);
    if (!tex || tex->m_rendererID == 0) {
        std::cerr << "[TextureManager] Failed to load texture from " << filePath << "\n";
        return false;
    }

    std::cout << "[TextureManager] Successfully loaded texture: " << name << " (ID " << tex->m_rendererID << ")\n";
    sheetMap[name] = { tex, 0, 0 };
    return true;
}

SharedPtr<Texture> TextureManager::getTexture(const string &name) const {
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
