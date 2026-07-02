#pragma once

#include "../GameComponents.hpp"

#include "../../engine/assets/Material.hpp"
#include "../../engine/components/CombatComponent.hpp"
#include "../../engine/components/MeshRenderer.hpp"
#include "../../engine/ecs/World.hpp"

#include <glm/glm.hpp>

#include <memory>

class Enemy {
public:
    Enemy(
        World& world,
        engine::ecs::EntityId parent,
        std::shared_ptr<MeshRenderer> bodyMesh,
        std::shared_ptr<Material> bodyMaterial);

    engine::ecs::EntityId entity() const { return root; }
    engine::ecs::EntityId getBody() const { return body; }

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void setTarget(engine::ecs::EntityId target);
    void update(float deltaTime);

    bool isAttacking() const;
    CombatComponent& getCombat();

    void setMoveSpeed(float speed);
    void setAttackRange(float range);
    void setDetectionRange(float range);

private:
    game::EnemyAI* ai();
    const game::EnemyAI* ai() const;

    World& worldRef;
    engine::ecs::EntityId root = engine::ecs::InvalidEntity;
    engine::ecs::EntityId body = engine::ecs::InvalidEntity;
};

std::unique_ptr<Enemy> CreateEnemy(
    World& world,
    engine::ecs::EntityId parent,
    std::shared_ptr<MeshRenderer> bodyMesh,
    std::shared_ptr<Material> bodyMaterial);
