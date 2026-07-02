#pragma once

#include "../ecs/EntityId.hpp"
#include "../ecs/World.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

struct PrefabData {
    std::string name;
    std::string modelPath;
    glm::vec3 defaultScale = glm::vec3(1.0f);
    bool hasAnimation = false;
    bool hasCollider = false;
    bool hasHealth = false;
    int defaultHealth = 100;
};

/*
 * PrefabLoader - Loads and caches prefab definitions for quick entity spawning
 * Prefabs are reusable entity templates that can be instantiated multiple times
 */
class PrefabLoader {
private:
    World* world;
    std::unordered_map<std::string, PrefabData> prefabs;
    std::unordered_map<std::string, std::shared_ptr<ModelAsset>> cachedModels;

public:
    PrefabLoader(World* world);
    ~PrefabLoader() = default;

    // Register a prefab definition
    void registerPrefab(const std::string& name, const PrefabData& data);
    
    // Load prefabs from JSON file
    bool loadPrefabsFromFile(const std::string& filePath);
    
    // Instantiate a prefab at position
    engine::ecs::EntityId instantiate(
        const std::string& prefabName,
        const glm::vec3& position,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity);
    engine::ecs::EntityId instantiate(
        const std::string& prefabName,
        const glm::vec3& position,
        const glm::quat& rotation,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity);
    
    // Check if prefab exists
    bool hasPrefab(const std::string& name) const;
    
    // Get all registered prefab names
    std::vector<std::string> getPrefabNames() const;
    
    // Clear all prefabs and cached models
    void clear();
};
