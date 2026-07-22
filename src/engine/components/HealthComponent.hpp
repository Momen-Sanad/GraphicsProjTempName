#pragma once

#include <glm/glm.hpp>

struct HealthComponent {
    int maxHP = 100;
    int hp = 100;
    float invulnTimer = 0.0f; // seconds left of invulnerability after hit
    float invulnDuration = 0.5f;

    bool dead = false;
    float respawnTimer = 0.0f;
    float respawnDelay = 2.0f;
    glm::vec3 spawnPos{0.0f, 0.0f, 0.0f};

    void set_spawn_point(const glm::vec3& pos);
    void update(float dt);
    bool apply_damage(int amount);
    bool ready_to_respawn() const;
    void respawn();
};
