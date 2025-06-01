#pragma once

#include <memory>
#include <string>
#include "entities/character.h"
#include "renderer/textureManager.h"
#include "entityManager.h"

/// Load a level from the specified JSON file.
/// Returns a pointer to the player Character if one is found, or nullptr otherwise.
std::shared_ptr<Character> loadLevelFromFile(
    const std::string& filename,
    std::shared_ptr<TextureManager> textureManager,
    EntityManager* entityManager);