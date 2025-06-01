#include "levelLoader.h"
#include "entities/platform.h"
#include "entityManager.h"
#include "renderer/textureManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::shared_ptr<Character> loadLevelFromFile(const std::string &filename, std::shared_ptr<TextureManager> textureManager, EntityManager* entityManager) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "[levelLoader] Failed to open level file: " << filename << std::endl;
        return nullptr;
    }

    json levelJson;
    inFile >> levelJson;


    std::shared_ptr<Character> playerCharacter = nullptr;

    for (const auto& obj : levelJson["objects"]) {
        if (!obj.contains("type") || !obj.contains("texture") ||
            !obj.contains("position") || !obj.contains("scale")) {
            std::cerr << "[LevelLoader] Skipping invalid object: missing required fields\n";
            continue;
        }
        std::cout << "Type: " << obj["type"] << ", Texture: " << obj["texture"] << std::endl;
        std::string type = obj["type"];
        std::string texture = obj["texture"];
        glm::vec3 position = glm::vec3(obj["position"][0], obj["position"][1], obj["position"][2]);
        glm::vec2 scale = glm::vec2(obj["scale"][0], obj["scale"][1]);

        // Load the texture if it's not already present
        if (!textureManager->hasTexture(texture)) {
            std::string fullPath = "assets/textures/" + texture + ".png";
            std::cout << "[LevelLoader] Attempting to load texture: " << fullPath << std::endl;
            std::cout << "[LevelLoader] textureManager ptr: " << textureManager.get() << std::endl;
            textureManager->loadTexture(texture, fullPath);
            std::cout << "[LevelLoader] textureManager ptr: " << textureManager.get() << std::endl;
        }

        if (!textureManager) {
            std::cerr << "[LevelLoader] ERROR: textureManager is NULL\n";
            abort(); // or return
        }

        auto texPtr = textureManager->getTexture(texture);
        std::cout << "[DEBUG] texPtr = " << texPtr.get()
          << ", ID = " << (texPtr ? texPtr->m_rendererID : 0) << std::endl;

        if (type == "platform") {
            std::string subtype = obj.value("subtype", "stationary");
            PlatformType pt = (subtype == "stationary") ? PlatformType::stationary : PlatformType::moving;
            entityManager->add<Platform>(pt, texPtr, position, 0.0f, scale, true);
        }
        else if (type == "character") {
            float speed = obj.value("speed", 2.5f);
            float mass = obj.value("mass", 0.2f);
            playerCharacter = entityManager->add<Character>(texPtr, position, speed, mass, scale);
        }
        else {
            std::cerr << "[levelLoader] Unknown object type: " << type << std::endl;
        }
    }

    std::cout << "[levelLoader] Level loaded: " << levelJson["levelName"] << std::endl;
    return playerCharacter;
}
