#pragma once
#include <vector>
#include <memory>
#include "Entity.hpp"

class EntityManager {
private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<Entity*> roots;

public:
    EntityManager() = default;

    Entity* createEntity();

    //overload it for a factory method
    Entity* createEntityWithParams(
        Entity* parent = nullptr,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.f, 0.f, 0.f, 0.f),
        const glm::vec3& scale    = glm::vec3(1.0f),
        MeshRenderer* mesh = nullptr,
        Material* material = nullptr
    );

    // cube-specific factory helper
    Entity* createCube(
        EntityManager& manager,
        Entity* parent,
        const glm::vec3& pos,
        const glm::vec3& scale,
        const glm::quat& rot,
        MeshRenderer* mesh,
        Material* material
    );

    void clear();
    
    void destroyEntity(Entity* entity);

    const std::vector<Entity*>& getRootEntities() const { return roots; }

    void drawAll(const glm::mat4& viewProj);

    const std::vector<Entity*>& getRoots() const {
        return roots;  
    }

    void renderEntityRecursive(Entity* e, const glm::mat4& VP);
};