#include "HealthComponent.hpp"
#include <algorithm>

void HealthComponent::set_spawn_point(const glm::vec3& pos) {
    spawnPos = pos;
}

void HealthComponent::update(float dt) {
    if (invulnTimer > 0.0f) {
        invulnTimer = std::max(0.0f, invulnTimer - dt);
    }
    if (dead && respawnTimer > 0.0f) {
        respawnTimer = std::max(0.0f, respawnTimer - dt);
    }
}

bool HealthComponent::apply_damage(int amount) {
    if (amount <= 0 || dead || invulnTimer > 0.0f) {
        return false;
    }

    hp -= amount;
    invulnTimer = invulnDuration;

    if (hp <= 0) {
        hp = 0;
        dead = true;
        respawnTimer = respawnDelay;
    }

    return true;
}

bool HealthComponent::ready_to_respawn() const {
    return dead && respawnTimer <= 0.0f;
}

void HealthComponent::respawn() {
    dead = false;
    hp = maxHP;
    invulnTimer = 0.0f;
    respawnTimer = 0.0f;
}
