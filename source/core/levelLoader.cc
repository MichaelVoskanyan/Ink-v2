#include "levelLoader.h"
#include "entities/platform.h"
#include "entityManager.h"
#include "renderer/textureManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
// #include <iostream>
#include <cstdio>

using json = nlohmann::json;

SharedPtr<Character> loadLevelFromFile(const std::string &filename,
                                             SharedPtr<TextureManager> textureManager,
                                             EntityManager *entityManager) {
    std::ifstream inFile(filename);
    if (!inFile) {
        fprintf(stderr, "[LevelLoader] Failed to open level file: %s\n", filename.c_str());
        return nullptr;
    }

    json levelJson;
    inFile >> levelJson;


    SharedPtr<Character> playerCharacter = nullptr;

    for (const auto &obj: levelJson["objects"]) {
        if (!obj.contains("type") || !obj.contains("texture") || !obj.contains("position") ||
            !obj.contains("scale")) {
            fprintf(stderr, "[LevelLoader] Skipping invalid object: missing required fields\n");
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
            printf("[LevelLoader] Attempting to load texture: %s", fullPath.c_str());
            textureManager->loadTexture(texture, fullPath);
        }

        if (!textureManager) {
            fprintf(stderr, "[LevelLoader] ERROR: textureManager is NULL\n");
            abort();  // or return
        }

        auto texPtr = textureManager->getTexture(texture);
//        std::cout << "[DEBUG] texPtr = " << texPtr.get()
//                  << ", ID = " << (texPtr ? texPtr->m_rendererID : 0) << std::endl;

        if (type == "platform") {
            std::string subtype = obj.value("subtype", "stationary");
            PlatformType pt =
                    (subtype == "stationary") ? PlatformType::stationary : PlatformType::moving;
            entityManager->add<Platform>(pt, texPtr, position, 0.0f, scale, true);
        } else if (type == "character") {
            float speed = obj.value("speed", 2.5f);
            float mass = obj.value("mass", 0.2f);
            playerCharacter = entityManager->add<Character>(texPtr, position, speed, mass, scale);
        } else {
            fprintf(stderr, "[LevelLoader] Unknown object type: %s\n", type.c_str());
        }
    }

//    std::cout << "[levelLoader] Level loaded: " << levelJson["levelName"] << std::endl;
    printf("[LevelLoader] Level Loaded.\n");
    return playerCharacter;
}
