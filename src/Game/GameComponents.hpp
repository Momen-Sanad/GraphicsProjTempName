#pragma once

#include "../engine/ecs/EntityId.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <vector>

struct PlayerInput {
    glm::vec2 move{0.0f};
    bool block = false;
    bool attack = false;
    bool dodge = false;
};

namespace game {

struct PlayerTag {};
struct EnemyTag {};
struct XpOrbTag {};

struct PlayerController {
    PlayerInput input{};
    glm::vec3 facing{0.0f, 0.0f, 1.0f};
    engine::ecs::EntityId body = engine::ecs::InvalidEntity;
    engine::ecs::EntityId weapon = engine::ecs::InvalidEntity;
    glm::quat weaponRestRotation{1.0f, 0.0f, 0.0f, 0.0f};

    bool blocking = false;

    float moveSpeed = 4.0f;
    float blockSpeedMultiplier = 0.4f;
    float dodgeSpeed = 10.0f;
    float dodgeDuration = 0.2f;
    float dodgeCooldown = 0.7f;
    float dodgeTimer = 0.0f;
    float dodgeCooldownTimer = 0.0f;

    float attackDuration = 0.25f;
    float attackCooldown = 0.35f;
    float attackTimer = 0.0f;
    float attackCooldownTimer = 0.0f;
};

struct EnemyAI {
    engine::ecs::EntityId target = engine::ecs::InvalidEntity;
    float moveSpeed = 2.5f;
    float attackRange = 2.0f;
    float detectionRange = 15.0f;
    float attackDuration = 0.3f;
    float attackCooldown = 1.0f;
    float attackTimer = 0.0f;
    float attackCooldownTimer = 0.0f;
    bool wasDead = false;
};

struct XPOrbComponent {
    glm::vec3 velocity{0.0f};
    float lifetime = 10.0f;
    int xpValue = 25;
};

struct PlayerProgress {
    int xp = 0;
    int level = 1;
    int xpToNextLevel = 100;
    int pendingUpgrades = 0;
    bool showUpgradeMenu = false;

    float moveSpeed = 4.0f;
    int damage = 25;
    float dodgeSpeed = 10.0f;
};

struct WaveState {
    int currentWave = 1;
    int enemiesPerWave = 1;
    int enemyDamageMultiplier = 1;
    int totalEnemiesKilled = 0;
    int enemiesAlive = 0;
    bool waveInProgress = true;
};

struct GameplayState {
    engine::ecs::EntityId root = engine::ecs::InvalidEntity;
    engine::ecs::EntityId island = engine::ecs::InvalidEntity;
    engine::ecs::EntityId player = engine::ecs::InvalidEntity;
    engine::ecs::EntityId playerVisual = engine::ecs::InvalidEntity;
    engine::ecs::EntityId testHouse = engine::ecs::InvalidEntity;
    engine::ecs::EntityId collisionSphere = engine::ecs::InvalidEntity;
    std::vector<engine::ecs::EntityId> enemies;
    std::vector<engine::ecs::EntityId> xpOrbs;
    PlayerProgress progress;
    WaveState waves;
};

} // namespace game
