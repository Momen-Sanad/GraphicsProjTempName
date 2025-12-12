#include "SceneManager.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace std;

SceneManager::SceneManager(World* world, ShaderManager* shaderManager) 
    : world(world), shaderManager(shaderManager) {
}

bool SceneManager::loadScene(const string& filePath) {
    // Load scene from JSON
    auto scene = SceneDeserializer::loadFromFile(filePath);
    if (!scene) {
        lastError = "Failed to load scene: " + SceneDeserializer::getLastError();
        return false;
    }

    // Clear existing scene
    clearScene();

    // Apply scene settings
    auto& camera = world->get_camera();
    if (scene->settings.cameraPosition.size() == 3) {
        camera.position = arrayToVec3(scene->settings.cameraPosition);
    }
    if (scene->settings.cameraDirection.size() == 3) {
        camera.direction = glm::normalize(arrayToVec3(scene->settings.cameraDirection));
    }

    // Create a cube mesh for scene entities
    Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));
    MeshRenderer* cubeRenderer = new MeshRenderer();
    cubeRenderer->upload(cubeMesh);

    // Create entities
    unordered_map<string, Entity*> entityMap;
    
    // First pass: create all entities
    for (const auto& entityData : scene->entities) {
        Entity* entity = world->createEntityWithParams(nullptr);
        entityMap[entityData.name] = entity;
        
        // Set transform
        entity->setPosition(arrayToVec3(entityData.position));
        entity->setRotation(eulerToQuat(entityData.rotation));
        entity->setScale(arrayToVec3(entityData.scale));
        
        // Assign cube mesh to all entities
        entity->setMesh(cubeRenderer);
    }
    
    // Second pass: set up hierarchy
    for (const auto& entityData : scene->entities) {
        Entity* entity = entityMap[entityData.name];
        
        // Set parent
        if (!entityData.parent.empty() && entityMap.find(entityData.parent) != entityMap.end()) {
            entity->setParent(entityMap[entityData.parent]);
        }
        
        // Create simple tinted material if tint is specified
        if (entityData.tint.size() == 4) {
            auto shader = shaderManager->loadShader("blackToWhite",
                string(SHADER_DIR) + "/blackToWhite.vert",
                string(SHADER_DIR) + "/blackToWhite.frag"
            );
            if (shader) {
                TintedMaterial* tintedMat = new TintedMaterial();
                tintedMat->setShader(shader);
                tintedMat->tint = glm::vec4(entityData.tint[0], entityData.tint[1], 
                                           entityData.tint[2], entityData.tint[3]);
                entity->setMaterial(tintedMat);
            }
        }
    }

    cout << "Scene loaded successfully: " << scene->name << endl;
    return true;
}

bool SceneManager::saveScene(const string& filePath) {
    auto sceneData = getCurrentSceneData();
    if (!sceneData) {
        lastError = "No scene data to save";
        return false;
    }
    
    if (!SceneSerializer::saveToFile(*sceneData, filePath)) {
        lastError = "Failed to save scene: " + SceneSerializer::getLastError();
        return false;
    }
    
    cout << "Scene saved successfully to: " << filePath << endl;
    return true;
}

unique_ptr<Scene> SceneManager::getCurrentSceneData() {
    auto scene = make_unique<Scene>();
    scene->name = "Current Scene";
    
    // Get camera settings
    auto& camera = world->get_camera();
    scene->settings.cameraPosition = {camera.position.x, camera.position.y, camera.position.z};
    scene->settings.cameraDirection = {camera.direction.x, camera.direction.y, camera.direction.z};
    
    // Extract entities from world
    extractEntitiesFromWorld(scene.get());
    
    return scene;
}

void SceneManager::extractEntitiesFromWorld(Scene* scene) {
    // Get all root entities from the world
    const auto& rootEntities = world->getRoots();
    
    // Extract each entity and its children recursively
    for (Entity* entity : rootEntities) {
        extractEntityRecursive(entity, scene, "");
    }
}

void SceneManager::extractEntityRecursive(Entity* entity, Scene* scene, const string& parentName) {
    if (!entity) return;
    
    Scene::EntityData entityData;
    
    // Generate a name for the entity (since Entity doesn't have names)
    entityData.name = "Entity_" + to_string(scene->entities.size());
    entityData.parent = parentName;
    
    // Extract transform data
    glm::vec3 pos = entity->getPosition();
    glm::quat rot = entity->getRotation();
    glm::vec3 scl = entity->getScale();
    
    entityData.position = {pos.x, pos.y, pos.z};
    entityData.scale = {scl.x, scl.y, scl.z};
    
    // Convert quaternion back to euler angles (approximate)
    entityData.rotation = quatToEuler(rot);
    
    // Extract material tint if it's a TintedMaterial
    Material* material = entity->getMaterial();
    if (material) {
        TintedMaterial* tintedMat = dynamic_cast<TintedMaterial*>(material);
        if (tintedMat) {
            entityData.tint = {tintedMat->tint.r, tintedMat->tint.g, tintedMat->tint.b, tintedMat->tint.a};
        }
    }
    
    // Add entity to scene
    scene->entities.push_back(entityData);
    
    // Recursively extract children
    const auto& children = entity->getChildren();
    for (Entity* child : children) {
        extractEntityRecursive(child, scene, entityData.name);
    }
}

void SceneManager::clearScene() {
    world->clear();
}

glm::vec3 SceneManager::arrayToVec3(const vector<float>& array) {
    if (array.size() >= 3) {
        return glm::vec3(array[0], array[1], array[2]);
    }
    return glm::vec3(0.0f);
}

glm::quat SceneManager::eulerToQuat(const vector<float>& euler) {
    if (euler.size() >= 3) {
        // Convert degrees to radians and create quaternion from euler angles
        float x = glm::radians(euler[0]);
        float y = glm::radians(euler[1]);
        float z = glm::radians(euler[2]);
        return glm::quat(glm::vec3(x, y, z));
    }
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

vector<float> SceneManager::quatToEuler(const glm::quat& quat) {
    // Convert quaternion to euler angles (in degrees)
    glm::vec3 euler = glm::eulerAngles(quat);
    return {glm::degrees(euler.x), glm::degrees(euler.y), glm::degrees(euler.z)};
}