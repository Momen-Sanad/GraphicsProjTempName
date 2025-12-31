#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../../engine/ecs/World.hpp"

class Enemy {
public:
    Enemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial);

    Entity* entity() const { return root; }
    void setPosition(const glm::vec3& p);

private:
    World& worldRef;
    Entity* root = nullptr;
    Entity* body = nullptr;

};

std::unique_ptr<Enemy> CreateEnemy(World& world,
                                   Entity* parent,
                                   MeshRenderer* bodyMesh,
                                   Material* bodyMaterial);
