#pragma once

#include <stl/ink_memory.h>
#include <string>
#include "entities/character.h"
#include "renderer/textureManager.h"
#include "entityManager.h"

/// Load a level from the specified JSON file.
/// Returns a pointer to the player Character if one is found, or nullptr otherwise.
SharedPtr<Character> loadLevelFromFile(
    const std::string& filename,
    SharedPtr<TextureManager> textureManager,
    EntityManager* entityManager);