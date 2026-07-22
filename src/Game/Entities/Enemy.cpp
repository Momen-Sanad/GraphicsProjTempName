#include "Enemy.hpp"

#include "../GameSystems.hpp"

#include "../../engine/components/HealthComponent.hpp"
#include "../../engine/components/HurtboxComponent.hpp"
#include "../../engine/ecs/EcsComponents.hpp"
#include "../../engine/ecs/Registry.hpp"

#include <algorithm>

Enemy::Enemy(
    World& world,
    engine::ecs::EntityId parent,
    std::shared_ptr<MeshRenderer> bodyMesh,
    std::shared_ptr<Material> bodyMaterial)
    : worldRef(world)
{
    root = worldRef.createEntity("Enemy", glm::vec3(0.0f));
    worldRef.setParent(root, parent);
    worldRef.registry().emplace<game::EnemyTag>(root);

    body = worldRef.createRenderable(
        "EnemyBody",
        std::move(bodyMesh),
        std::move(bodyMaterial),
        root,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 2.0f, 1.0f));

    worldRef.registry().emplace<game::EnemyAI>(root);

    auto& combat = worldRef.registry().emplace<CombatComponent>(root);
    combat.damage = 15;
    combat.hitbox.halfExtents = {0.5f, 0.8f, 0.5f};
    combat.hitbox.localOffset = {0.0f, 1.0f, 0.8f};

    auto& health = worldRef.registry().emplace<HealthComponent>(root);
    health.maxHP = 100;
    health.hp = 100;
    health.invulnDuration = 0.35f;
    health.respawnDelay = 2.0f;

    auto& hurtbox = worldRef.registry().emplace<HurtboxComponent>(root);
    hurtbox.halfExtents = {0.5f, 1.0f, 0.5f};
    hurtbox.localOffset = {0.0f, 1.0f, 0.0f};

    auto& collider = worldRef.registry().emplace<engine::ecs::ColliderData>(root);
    collider.halfExtents = glm::vec3(0.4f, 1.0f, 0.4f);
    collider.localOffset = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Enemy::setPosition(const glm::vec3& position)
{
    game::setLocalPosition(worldRef, root, position);
    if (auto* health = worldRef.registry().get<HealthComponent>(root)) {
        health->set_spawn_point(position);
    }
}

glm::vec3 Enemy::getPosition() const
{
    return game::worldPosition(const_cast<World&>(worldRef), root);
}

void Enemy::setTarget(engine::ecs::EntityId target)
{
    if (auto* data = ai()) {
        data->target = target;
    }
}

void Enemy::update(float deltaTime)
{
    game::updateEnemyAI(worldRef, root, deltaTime);
}

bool Enemy::isAttacking() const
{
    const auto* data = ai();
    return data ? data->attackTimer > 0.0f : false;
}

CombatComponent& Enemy::getCombat()
{
    return worldRef.registry().ensure<CombatComponent>(root);
}

void Enemy::setMoveSpeed(float speed)
{
    if (auto* data = ai()) {
        data->moveSpeed = std::max(0.0f, speed);
    }
}

void Enemy::setAttackRange(float range)
{
    if (auto* data = ai()) {
        data->attackRange = std::max(0.0f, range);
    }
}

void Enemy::setDetectionRange(float range)
{
    if (auto* data = ai()) {
        data->detectionRange = std::max(0.0f, range);
    }
}

game::EnemyAI* Enemy::ai()
{
    return worldRef.registry().get<game::EnemyAI>(root);
}

const game::EnemyAI* Enemy::ai() const
{
    return worldRef.registry().get<game::EnemyAI>(root);
}

std::unique_ptr<Enemy> CreateEnemy(
    World& world,
    engine::ecs::EntityId parent,
    std::shared_ptr<MeshRenderer> bodyMesh,
    std::shared_ptr<Material> bodyMaterial)
{
    return std::make_unique<Enemy>(world, parent, std::move(bodyMesh), std::move(bodyMaterial));
}
