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

bool TextureManager::loadTexture(const string &name, const string &filePath) {
    sheetMap[name] = { std::move(std::make_shared<Texture>(filePath)), 0, 0 };
    return true;
}

shared_ptr<Texture> TextureManager::getTexture(const string &name) const {
    auto it = sheetMap.find(name);
    return it != sheetMap.end() ? it->second.texture : 0;
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
