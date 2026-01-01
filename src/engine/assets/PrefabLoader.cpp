#include "PrefabLoader.hpp"
#include "MeshLoader.hpp"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PrefabLoader::PrefabLoader(World* world) : world(world) {}

void PrefabLoader::registerPrefab(const std::string& name, const PrefabData& data) {
    prefabs[name] = data;
    std::cout << "[PrefabLoader] Registered prefab: " << name << std::endl;
}

bool PrefabLoader::loadPrefabsFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[PrefabLoader] Failed to open: " << filePath << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        for (auto& [name, data] : j["prefabs"].items()) {
            PrefabData prefab;
            prefab.name = name;
            prefab.modelPath = data.value("model", "");
            
            if (data.contains("scale")) {
                auto& s = data["scale"];
                prefab.defaultScale = glm::vec3(s[0], s[1], s[2]);
            }
            
            prefab.hasAnimation = data.value("animated", false);
            prefab.hasCollider = data.value("collider", false);
            prefab.hasHealth = data.value("health", false);
            prefab.defaultHealth = data.value("maxHP", 100);

            registerPrefab(name, prefab);
        }
        
        std::cout << "[PrefabLoader] Loaded " << prefabs.size() << " prefabs from " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[PrefabLoader] JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

Entity* PrefabLoader::instantiate(const std::string& prefabName, const glm::vec3& position, Entity* parent) {
    return instantiate(prefabName, position, glm::quat(1, 0, 0, 0), parent);
}

Entity* PrefabLoader::instantiate(const std::string& prefabName, const glm::vec3& position, const glm::quat& rotation, Entity* parent) {
    auto it = prefabs.find(prefabName);
    if (it == prefabs.end()) {
        std::cerr << "[PrefabLoader] Prefab not found: " << prefabName << std::endl;
        return nullptr;
    }

    const PrefabData& data = it->second;
    
    // Create entity
    Entity* entity = world->createEntityWithParams(parent, position, rotation, data.defaultScale);
    entity->setName(data.name);

    // Load model if specified
    if (!data.modelPath.empty()) {
        // Check cache first
        if (cachedModels.find(data.modelPath) == cachedModels.end()) {
            ModelData* model = MeshLoader::load_gltf(data.modelPath.c_str());
            if (model) {
                cachedModels[data.modelPath] = std::shared_ptr<ModelData>(model);
            }
        }
        
        auto modelIt = cachedModels.find(data.modelPath);
        if (modelIt != cachedModels.end()) {
            entity->setModelData(modelIt->second);
        }
    }

    // Note: Health/Combat components should be added by game code after instantiation
    // as they are data structs, not Component subclasses

    std::cout << "[PrefabLoader] Instantiated: " << prefabName << " at (" 
              << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    
    return entity;
}

bool PrefabLoader::hasPrefab(const std::string& name) const {
    return prefabs.find(name) != prefabs.end();
}

std::vector<std::string> PrefabLoader::getPrefabNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : prefabs) {
        names.push_back(name);
    }
    return names;
}

void PrefabLoader::clear() {
    prefabs.clear();
    cachedModels.clear();
}
