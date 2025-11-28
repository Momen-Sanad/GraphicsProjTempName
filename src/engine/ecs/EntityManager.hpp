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
    void destroyEntity(Entity* entity);

    const std::vector<Entity*>& getRootEntities() const { return roots; }

    void drawAll(const glm::mat4& viewProj);
};