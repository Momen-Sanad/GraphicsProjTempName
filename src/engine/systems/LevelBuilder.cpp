#include "LevelBuilder.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

LevelBuilder::LevelBuilder(World* world, PrefabLoader* prefabLoader)
    : world(world), prefabLoader(prefabLoader) {}

bool LevelBuilder::loadLevel(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[LevelBuilder] Failed to open: " << filePath << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        currentLevel = LevelData();
        currentLevel.name = j.value("name", "Unnamed Level");

        // Parse player spawn
        if (j.contains("playerSpawn")) {
            auto& ps = j["playerSpawn"];
            currentLevel.playerSpawn = glm::vec3(ps[0], ps[1], ps[2]);
        }

        // Parse camera start
        if (j.contains("cameraStart")) {
            auto& cs = j["cameraStart"];
            currentLevel.cameraStart = glm::vec3(cs[0], cs[1], cs[2]);
        }

        // Parse spawn points
        if (j.contains("entities")) {
            for (auto& entity : j["entities"]) {
                SpawnPoint sp;
                sp.prefabName = entity.value("prefab", "");
                
                if (entity.contains("position")) {
                    auto& p = entity["position"];
                    sp.position = glm::vec3(p[0], p[1], p[2]);
                }
                
                if (entity.contains("rotation")) {
                    auto& r = entity["rotation"];
                    // Euler angles in degrees
                    glm::vec3 euler(glm::radians((float)r[0]), 
                                   glm::radians((float)r[1]), 
                                   glm::radians((float)r[2]));
                    sp.rotation = glm::quat(euler);
                }
                
                if (entity.contains("scale")) {
                    auto& s = entity["scale"];
                    if (s.is_array()) {
                        sp.scale = glm::vec3(s[0], s[1], s[2]);
                    } else {
                        float uniform = s.get<float>();
                        sp.scale = glm::vec3(uniform);
                    }
                }

                currentLevel.spawnPoints.push_back(sp);
            }
        }

        std::cout << "[LevelBuilder] Loaded level: " << currentLevel.name 
                  << " with " << currentLevel.spawnPoints.size() << " entities" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[LevelBuilder] JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

bool LevelBuilder::buildLevel() {
    std::cout << "[LevelBuilder] Building level: " << currentLevel.name << std::endl;
    
    levelEntities.clear();
    
    for (const auto& sp : currentLevel.spawnPoints) {
        if (sp.prefabName.empty()) continue;
        
        Entity* entity = prefabLoader->instantiate(sp.prefabName, sp.position, sp.rotation);
        if (entity) {
            entity->setScale(sp.scale);
            levelEntities.push_back(entity);
        }
    }

    std::cout << "[LevelBuilder] Built " << levelEntities.size() << " entities" << std::endl;
    return true;
}

void LevelBuilder::unloadLevel() {
    std::cout << "[LevelBuilder] Unloading level: " << currentLevel.name << std::endl;
    
    for (Entity* entity : levelEntities) {
        if (entity) {
            world->getEntityManager().destroyEntity(entity);
        }
    }
    
    levelEntities.clear();
    currentLevel = LevelData();
}
