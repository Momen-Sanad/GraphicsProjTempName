#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "Entity.hpp"
#include "../components/Camera.hpp"
#include "EntityManager.hpp"

class World {
public:
    World();
    ~World() = default;

    // --- Entity Factory ---
    Entity* createEntityWithParams(Entity* parent = nullptr,
                                   const glm::vec3& position = glm::vec3(0.f),
                                   const glm::quat& rotation = glm::quat(1,0,0,0),
                                   const glm::vec3& scale = glm::vec3(1.f),
                                   MeshRenderer* mesh = nullptr,
                                   Material* material = nullptr);
    
    Entity* add_entity();
    void removeEntity(Entity* entity);
    void clear();
    
    // --- Accessors ---
    Camera& get_camera() { return camera; }
    const std::vector<Entity*>& get_entities() const { return manager.getRootEntities(); }
    EntityManager& getEntityManager() { return manager; }
    
    const std::vector<Entity*>& getRoots() const { return manager.getRoots(); }
    
private:
    Camera camera;
    EntityManager manager;
};

#endif
