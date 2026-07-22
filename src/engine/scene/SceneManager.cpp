#include "SceneManager.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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
   auto cubeRenderer = world->assets().createMeshRenderer("scene:cube", cubeMesh);

   // Create entities
   unordered_map<string, engine::ecs::EntityId> entityMap;
   
   // First pass: create all entities
   for (const auto& entityData : scene->entities) {
       engine::ecs::EntityId entity = world->createEntity(
           entityData.name,
           arrayToVec3(entityData.position),
           eulerToQuat(entityData.rotation),
           arrayToVec3(entityData.scale));
       entityMap[entityData.name] = entity;

       world->registry().emplace<engine::ecs::Renderable>(
           entity,
           engine::ecs::Renderable{cubeRenderer, nullptr});
   }
   
   // Second pass: set up hierarchy
   for (const auto& entityData : scene->entities) {
       engine::ecs::EntityId entity = entityMap[entityData.name];
       
       // Set parent
       if (!entityData.parent.empty() && entityMap.find(entityData.parent) != entityMap.end()) {
           world->setParent(entity, entityMap[entityData.parent]);
       }
       
       // Create simple tinted material if tint is specified
       if (entityData.tint.size() == 4) {
           auto shader = shaderManager->loadShader("blackToWhite",
                             string(SHADER_DIR) + "/blackToWhite.vert",
                             string(SHADER_DIR) + "/blackToWhite.frag"
           );

           if (shader) {
               auto tintedMat = std::make_shared<TintedMaterial>();
               tintedMat->setShader(shader);
               tintedMat->tint = glm::vec4(entityData.tint[0], entityData.tint[1], 
                                          entityData.tint[2], entityData.tint[3]);
               if (auto* renderable = world->registry().get<engine::ecs::Renderable>(entity)) {
                   renderable->material = tintedMat;
               }
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
   
   // TODO: Extract entities from world
   // This would require iterating through world entities and converting them back to EntityData
   // For now, return empty scene with just camera data
   // we must implement this to make scene serialization possible
       
   return scene;
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
