#pragma once

#include "Scene.hpp"
#include "SceneDeserializer.hpp"
#include "SceneSerializer.hpp"
#include "../ecs/World.hpp"
#include "../ecs/ShaderManager.hpp"
#include "../assets/TintedMaterial.hpp"
#include <memory>
#include <unordered_map>

using namespace std;

/**
 * Simple scene manager that loads scenes into the existing World
 */
class SceneManager {
public:
    SceneManager(World* world, ShaderManager* shaderManager);
    ~SceneManager() = default;

    bool loadScene(const string& filePath);
    bool saveScene(const string& filePath);
    void clearScene();
    const string& getLastError() const { return lastError; }
    
    // Get current scene state from world
    unique_ptr<Scene> getCurrentSceneData();

private:
    World* world;
    ShaderManager* shaderManager;
    string lastError;
    
    // Helper functions
    glm::vec3 arrayToVec3(const vector<float>& array);
    glm::quat eulerToQuat(const vector<float>& euler);
    vector<float> quatToEuler(const glm::quat& quat);
    
    // Entity extraction helpers
    void extractEntitiesFromWorld(Scene* scene);
    void extractEntityRecursive(Entity* entity, Scene* scene, const string& parentName);
};