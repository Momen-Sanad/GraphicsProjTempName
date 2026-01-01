#pragma once

#include <algorithm>
#include <functional>
#include <glm/glm.hpp>

#include "../ecs/Component.hpp"

class World;
class Entity;

class SpawnPointComponent final : public Component {
public:
    using SpawnFunc = std::function<Entity*(World&, const glm::vec3&)>;

    explicit SpawnPointComponent(Entity* owner, SpawnFunc spawn = nullptr)
        : Component(owner), spawnFn(std::move(spawn)) {}

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

    void update(Entity&, float) override {}
    void render(Entity&) override {}

    void mark_despawned(int count = 1) {
        aliveCount = std::max(0, aliveCount - std::max(0, count));
    }
};
