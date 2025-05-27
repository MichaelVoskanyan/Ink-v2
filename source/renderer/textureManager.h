// textureManager.h
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
using namespace std;

/// Holds everything you need to draw one frame
struct SpriteInfo {
    uint32_t texID;  // GL texture handle
    int texWidth, texHeight;
    int spriteWidth, spriteHeight;
    int frameIndex;
};

class TextureManager {
public:
    /// singleton
    static shared_ptr<TextureManager> instance();

    /// load a plain GL texture under 'name'
    bool loadTexture(const string &name, const string &filePath);

    /// get a raw texture ID (0 if not found)
    uint32_t getTexture(const string &name) const;

    /// load a sprite-sheet under 'sheetName' (you still bind by sheet)
    /// texW/texH = full sheet dims, sW/sH = one frame dims
    bool loadSpriteSheet(const string &sheetName, const string &filePath, int texW, int texH,
                         int sW, int sH);

    /// register one frame by its own key, pointing to a previously loaded sheet
    /// e.g. registerSprite("character_walk_2", "character_walk", 2);
    bool registerSprite(const string &spriteName, const string &sheetName, int frameIndex);

    /// fetch everything you need to draw that frame
    /// nullptr if spriteName wasn’t registered
    const SpriteInfo *getSpriteInfo(const string &spriteName) const;

    // no copies or moves
    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;
    TextureManager(TextureManager &&) = delete;
    TextureManager &operator=(TextureManager &&) = delete;

    TextureManager() = default;
    ~TextureManager();

private:
    struct Sheet {
        uint32_t texID;
        int texW, texH;
        int sW, sH;
    };

    unordered_map<string, Sheet> sheetMap;
    unordered_map<string, SpriteInfo> spriteMap;
};
