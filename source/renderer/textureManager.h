#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include "texture.h"

using namespace std;

/// Holds everything you need to draw one frame
struct SpriteInfo {
    shared_ptr<Texture> texture;   // pointer to the loaded texture
    int spriteWidth;               // width of a single frame
    int spriteHeight;              // height of a single frame
    int frameIndex;                // index of the frame within the sheet
};

class TextureManager {
public:
    /// Get the singleton instance
    static shared_ptr<TextureManager> instance();

    /// Load a plain texture under 'name'
    bool loadTexture(const string &name, const string &filePath);

    /// Retrieve the Texture pointer you previously loaded (or nullptr)
    shared_ptr<Texture> getTexture(const string &name) const;

    /**
     * Load a sprite sheet under 'sheetName'
     * spriteWidth/spriteHeight = dimensions of each frame
     */
    bool loadSpriteSheet(const string &sheetName,
                         const string &filePath,
                         int spriteWidth,
                         int spriteHeight);

    /**
     * Register a single frame by its own key, pointing to a loaded sheet
     * e.g. registerSprite("hero_run_2", "hero_run", 2);
     */
    bool registerSprite(const string &spriteName,
                        const string &sheetName,
                        int frameIndex);

    bool hasTexture(const string &textureName);

    /// Fetch everything needed to draw that frame (nullptr if missing)
    const SpriteInfo *getSpriteInfo(const string &spriteName) const;

    // disable copy/move
    TextureManager(const TextureManager &) = delete;
    TextureManager &operator=(const TextureManager &) = delete;
    TextureManager(TextureManager &&) = delete;
    TextureManager &operator=(TextureManager &&) = delete;

    TextureManager() = default;
    ~TextureManager() = default;

private:
    struct Sheet {
        shared_ptr<Texture> texture; // wrapper holds width/height
        int spriteWidth;
        int spriteHeight;
    };

    unordered_map<string, Sheet> sheetMap;      // sheetName -> sheet data
    unordered_map<string, SpriteInfo> spriteMap; // spriteName -> frame info
};
