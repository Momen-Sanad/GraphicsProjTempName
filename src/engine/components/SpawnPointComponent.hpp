#pragma once

#include <algorithm>
#include <functional>
#include <glm/glm.hpp>

#include "../ecs/EntityId.hpp"

class World;

struct SpawnPointComponent {
    using SpawnFunc = std::function<engine::ecs::EntityId(World&, const glm::vec3&)>;

    explicit SpawnPointComponent(SpawnFunc spawn = nullptr)
        : spawnFn(std::move(spawn)) {}

    // Configurable spawn behavior
    glm::vec3 localOffset{0.0f, 0.0f, 0.0f};
    float respawnDelay = 0.0f;
    int maxAlive = 1;   // -1 for unlimited
    int maxSpawns = -1; // -1 for unlimited
    bool spawnOnStart = true;
    bool active = true;

    // Runtime state
    float timer = 0.0f;
    int aliveCount = 0;
    int totalSpawned = 0;
    bool initialized = false;

    SpawnFunc spawnFn;

    void mark_despawned(int count = 1) {
        aliveCount = std::max(0, aliveCount - std::max(0, count));
    }
};
