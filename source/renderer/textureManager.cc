// textureManager.cpp
#include "textureManager.h"
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>  
#include <iostream>
using namespace std;
// include your image‐loader (stb_image.h) and GL headers

std::shared_ptr<textureManager_t> textureManager_t::instance() {
    static auto inst = std::make_shared<textureManager_t>();
    return inst;
}

textureManager_t::~textureManager_t() = default;

bool textureManager_t::loadTexture(const string& name,
                                   const string& filePath)
{
    int w, h, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &w, &h, &channels, 4);
    if (!data) {
        cerr << "Failed to load image: " << filePath << "\n";
        return false;
    }

    uint32_t tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,
                 0, GL_RGBA,
                 w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE,
                 data);
    stbi_image_free(data);

    // if someone later calls getTexture(name), return this
    sheetMap[name] = { tex, w, h, /*sW*/0, /*sH*/0 };
    return true;
}

uint32_t textureManager_t::getTexture(const string& name) const {
    auto it = sheetMap.find(name);
    return it != sheetMap.end() ? it->second.texID : 0;
}

bool textureManager_t::loadSpriteSheet(const string& sheetName,
                                       const string& filePath,
                                       int texW, int texH,
                                       int sW, int sH)
{
    if (!loadTexture(sheetName, filePath))
        return false;

    // overwrite the sheet entry with correct sprite dims
    auto& sh = sheetMap[sheetName];
    sh.texW = texW;
    sh.texH = texH;
    sh.sW   = sW;
    sh.sH   = sH;
    return true;
}

bool textureManager_t::registerSprite(const string& spriteName,
                                      const string& sheetName,
                                      int frameIndex)
{
    auto it = sheetMap.find(sheetName);
    if (it == sheetMap.end()) {
        cerr << "No sheet loaded as \"" << sheetName << "\"\n";
        return false;
    }
    const auto& sh = it->second;
    spriteMap[spriteName] = {
        sh.texID,
        sh.texW, sh.texH,
        sh.sW,   sh.sH,
        frameIndex
    };
    return true;
}

const spriteInfo_t* textureManager_t::getSpriteInfo(const string& spriteName) const {
    auto it = spriteMap.find(spriteName);
    return it != spriteMap.end() ? &it->second : nullptr;
}
