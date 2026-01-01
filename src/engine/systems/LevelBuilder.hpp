#pragma once

#include "../ecs/World.hpp"
#include "../assets/PrefabLoader.hpp"
#include <string>
#include <vector>

struct SpawnPoint {
    std::string prefabName;
    glm::vec3 position;
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1.0f);
};

struct LevelData {
    std::string name;
    std::vector<SpawnPoint> spawnPoints;
    glm::vec3 playerSpawn = glm::vec3(0, 0, 0);
    glm::vec3 cameraStart = glm::vec3(0, 5, 10);
};

/*
 * LevelBuilder - Builds game levels from JSON definitions
 * Uses PrefabLoader to instantiate entities at defined spawn points
 */
class LevelBuilder {
private:
    World* world;
    PrefabLoader* prefabLoader;
    LevelData currentLevel;
    std::vector<Entity*> levelEntities;

public:
    LevelBuilder(World* world, PrefabLoader* prefabLoader);
    ~LevelBuilder() = default;

    // Load level from JSON file
    bool loadLevel(const std::string& filePath);
    
    // Build the loaded level (instantiate all entities)
    bool buildLevel();
    
    // Unload current level (destroy all level entities)
    void unloadLevel();
    
    // Get level info
    const LevelData& getLevelData() const { return currentLevel; }
    glm::vec3 getPlayerSpawn() const { return currentLevel.playerSpawn; }
    glm::vec3 getCameraStart() const { return currentLevel.cameraStart; }
    
    // Get spawned entities
    const std::vector<Entity*>& getLevelEntities() const { return levelEntities; }
};
