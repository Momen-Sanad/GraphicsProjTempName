#pragma once

#include "Scene.hpp"
#include "SceneDeserializer.hpp"
#include "SceneSerializer.hpp"
#include "../ecs/World.hpp"
#include "../assets/AssetManager.hpp"
#include "../assets/TintedMaterial.hpp"
#include <memory>
#include <unordered_map>

// Update scene/SceneDeserializer / SceneSerializer to read/write Light components in JSON.
using namespace std;

/*
 * Simple scene manager that loads scenes into the existing World
 */
class SceneManager {
private:
    World* world;
    AssetManager* assetManager;
    string lastError;
    
    // Helper functions
    glm::vec3 arrayToVec3(const vector<float>& array);
    glm::quat eulerToQuat(const vector<float>& euler);

public:
    SceneManager(World* world, AssetManager* assetManager);
    ~SceneManager() = default;

    bool loadScene(const string& filePath);
    bool saveScene(const string& filePath);
    void clearScene();
    const string& getLastError() const { return lastError; }
    
    // Get current scene state from world
    unique_ptr<Scene> getCurrentSceneData();
};
