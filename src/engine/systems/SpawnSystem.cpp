#include "SpawnSystem.hpp"

#include "../components/SpawnPointComponent.hpp"
#include "../ecs/Entity.hpp"
#include "../ecs/World.hpp"

void SpawnSystem::update(World& world, float deltaTime) {
    for (Entity* root : world.getEntityManager().getRoots()) {
        if (!root) {
            continue;
        }
        update_entity_recursive(*root, world, deltaTime);
    }
}

void SpawnSystem::update_entity_recursive(Entity& entity, World& world, float deltaTime) {
    if (auto* spawnPoint = entity.getComponent<SpawnPointComponent>()) {
        update_spawn_point(entity, world, *spawnPoint, deltaTime);
    }

    for (Entity* child : entity.getChildren()) {
        if (child) {
            update_entity_recursive(*child, world, deltaTime);
        }
    }
}

void SpawnSystem::update_spawn_point(Entity& entity,
                                    World& world,
                                    SpawnPointComponent& spawnPoint,
                                    float deltaTime) {
    if (!spawnPoint.active) {
        return;
    }

    if (!spawnPoint.initialized) {
        spawnPoint.timer = spawnPoint.spawnOnStart ? 0.0f : spawnPoint.respawnDelay;
        spawnPoint.initialized = true;
    }

    if (spawnPoint.maxSpawns >= 0 && spawnPoint.totalSpawned >= spawnPoint.maxSpawns) {
        return;
    }

    if (spawnPoint.maxAlive >= 0 && spawnPoint.aliveCount >= spawnPoint.maxAlive) {
        return;
    }

    spawnPoint.timer -= deltaTime;
    if (spawnPoint.timer > 0.0f) {
        return;
    }

    if (spawnPoint.spawnFn) {
        const glm::vec3 basePos = glm::vec3(entity.getWorldMatrix()[3]);
        const glm::vec3 spawnPos = basePos + spawnPoint.localOffset;

        Entity* spawned = spawnPoint.spawnFn(world, spawnPos);
        if (spawned) {
            spawnPoint.aliveCount += 1;
            spawnPoint.totalSpawned += 1;
        }
    }

    spawnPoint.timer = spawnPoint.respawnDelay;
}
