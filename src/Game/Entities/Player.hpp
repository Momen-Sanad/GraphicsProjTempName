//example usage
/*
#pragma once
#include <memory>
#include "../../engine/ecs/Entity.hpp"
#include "../../engine/components/HealthComponent.hpp"
#include "../../engine/components/MovementComponent.hpp"
// include MeshRenderer, Transform, Hitbox etc.

class Player {
public:
    Player(World& world);
    ~Player() = default;

    Entity* entity() { return pEntity; }

    // convenience
    void setPosition(const glm::vec3& p);
    void applyInput(const InputState& in);

private:
    World& worldRef;
    Entity* pEntity = nullptr;       // raw pointer to entity managed by world
    Entity* body = nullptr;
    Entity* weapon = nullptr;
};
*/