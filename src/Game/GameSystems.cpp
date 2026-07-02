#include "GameSystems.hpp"

#include "../engine/ecs/EcsComponents.hpp"
#include "../engine/ecs/Registry.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/systems/TransformSystem.hpp"

#include <algorithm>
#include <cmath>

namespace game {

glm::vec3 localPosition(World& world, engine::ecs::EntityId entity)
{
    const auto* transform = world.registry().get<engine::ecs::Transform>(entity);
    return transform ? transform->position : glm::vec3(0.0f);
}

glm::vec3 worldPosition(World& world, engine::ecs::EntityId entity)
{
    TransformSystem::updateWorldTransforms(world.registry());
    const auto* transform = world.registry().get<engine::ecs::Transform>(entity);
    return transform ? glm::vec3(transform->worldMatrix[3]) : glm::vec3(0.0f);
}

glm::vec3 forward(World& world, engine::ecs::EntityId entity)
{
    const auto* transform = world.registry().get<engine::ecs::Transform>(entity);
    if (!transform) {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }

    glm::vec3 result = glm::vec3(glm::mat4_cast(transform->rotation) * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    if (glm::length(result) <= 0.0001f) {
        return glm::vec3(0.0f, 0.0f, 1.0f);
    }
    return glm::normalize(result);
}

void setLocalPosition(World& world, engine::ecs::EntityId entity, const glm::vec3& position)
{
    if (auto* transform = world.registry().get<engine::ecs::Transform>(entity)) {
        transform->position = position;
        transform->dirty = true;
    }
}

void updatePlayerController(World& world, engine::ecs::EntityId player, float deltaTime)
{
    auto* controller = world.registry().get<PlayerController>(player);
    auto* transform = world.registry().get<engine::ecs::Transform>(player);
    if (!controller || !transform) {
        return;
    }

    controller->blocking = controller->input.block;
    controller->attackTimer = std::max(0.0f, controller->attackTimer - deltaTime);
    controller->dodgeTimer = std::max(0.0f, controller->dodgeTimer - deltaTime);
    controller->attackCooldownTimer = std::max(0.0f, controller->attackCooldownTimer - deltaTime);
    controller->dodgeCooldownTimer = std::max(0.0f, controller->dodgeCooldownTimer - deltaTime);

    if (controller->input.attack &&
        !controller->blocking &&
        controller->attackCooldownTimer <= 0.0f &&
        controller->attackTimer <= 0.0f) {
        controller->attackTimer = controller->attackDuration;
        controller->attackCooldownTimer = controller->attackCooldown;
    }

    if (controller->input.dodge &&
        controller->dodgeCooldownTimer <= 0.0f &&
        controller->dodgeTimer <= 0.0f) {
        controller->dodgeTimer = controller->dodgeDuration;
        controller->dodgeCooldownTimer = controller->dodgeCooldown;
    }

    glm::vec3 move(controller->input.move.x, 0.0f, controller->input.move.y);
    const float moveLen = glm::length(move);
    if (moveLen > 0.001f) {
        move /= moveLen;
        controller->facing = move;
    }

    const float speed = controller->blocking
        ? controller->moveSpeed * controller->blockSpeedMultiplier
        : controller->moveSpeed;

    if (controller->dodgeTimer > 0.0f) {
        transform->position += controller->facing * controller->dodgeSpeed * deltaTime;
        transform->dirty = true;
    } else if (moveLen > 0.001f) {
        transform->position += move * speed * deltaTime;
        transform->dirty = true;
    }

    if (moveLen > 0.001f) {
        const float yaw = std::atan2(controller->facing.x, controller->facing.z);
        transform->rotation = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        transform->dirty = true;
    }

    if (auto* weapon = world.registry().get<engine::ecs::Transform>(controller->weapon)) {
        if (controller->attackTimer > 0.0f) {
            const float t = 1.0f - (controller->attackTimer / controller->attackDuration);
            const float swing = std::sin(t * 3.1415926f);
            weapon->rotation =
                controller->weaponRestRotation *
                glm::angleAxis(-swing * 1.2f, glm::vec3(0.0f, 1.0f, 0.0f));
        } else {
            weapon->rotation = controller->weaponRestRotation;
        }
        weapon->dirty = true;
    }
}

void updateEnemyAI(World& world, engine::ecs::EntityId enemy, float deltaTime)
{
    auto* ai = world.registry().get<EnemyAI>(enemy);
    auto* transform = world.registry().get<engine::ecs::Transform>(enemy);
    const auto* health = world.registry().get<HealthComponent>(enemy);
    if (!ai || !transform || (health && health->dead) || !world.registry().isAlive(ai->target)) {
        return;
    }

    ai->attackTimer = std::max(0.0f, ai->attackTimer - deltaTime);
    ai->attackCooldownTimer = std::max(0.0f, ai->attackCooldownTimer - deltaTime);

    glm::vec3 toTarget = worldPosition(world, ai->target) - worldPosition(world, enemy);
    toTarget.y = 0.0f;
    const float distance = glm::length(toTarget);

    if (distance > ai->detectionRange) {
        return;
    }

    if (distance > 0.1f) {
        const glm::vec3 dir = glm::normalize(toTarget);
        const float yaw = std::atan2(dir.x, dir.z);
        transform->rotation = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        transform->dirty = true;
    }

    if (distance > ai->attackRange) {
        transform->position += glm::normalize(toTarget) * ai->moveSpeed * deltaTime;
        transform->dirty = true;
    } else if (ai->attackCooldownTimer <= 0.0f) {
        ai->attackTimer = ai->attackDuration;
        ai->attackCooldownTimer = ai->attackCooldown;
    }
}

DefenseState defenseState(World& world, engine::ecs::EntityId player)
{
    DefenseState state;
    if (const auto* controller = world.registry().get<PlayerController>(player)) {
        state.blocking = controller->blocking;
        state.dodging = controller->dodgeTimer > 0.0f;
        state.dodgeTimer = state.dodging ? 0.0f : 1.0f;
        state.dodgeWindow = controller->dodgeDuration;
    }
    return state;
}

bool grantExperience(PlayerProgress& progress, int xpValue)
{
    bool leveled = false;
    progress.xp += std::max(0, xpValue);

    while (progress.xp >= progress.xpToNextLevel) {
        progress.xp -= progress.xpToNextLevel;
        progress.level++;
        progress.xpToNextLevel = 100 * progress.level;
        progress.pendingUpgrades++;
        progress.showUpgradeMenu = true;
        leveled = true;
    }

    return leveled;
}

} // namespace game
