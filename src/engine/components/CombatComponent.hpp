#pragma once

#include <glm/glm.hpp>
#include "HealthComponent.hpp"
#include "HitboxComponent.hpp"
#include "HurtboxComponent.hpp"

struct DefenseState {
    bool blocking = false;
    bool dodging = false;
    float dodgeTimer = 0.0f;  // Time since dodge started
    float dodgeWindow = 0.5f; // Window for half damage
};

struct CombatComponent {
    HitboxComponent hitbox;
    int damage = 25;
    bool hitThisSwing = false;
    bool wasAttacking = false;

    void update_attack_state(bool isAttacking);
    bool resolve_attack(bool isAttacking,
                        const glm::vec3& attackerPos,
                        const glm::vec3& attackerForward,
                        const HurtboxComponent& targetHurtbox,
                        const glm::vec3& targetPos,
                        HealthComponent& targetHealth,
                        const DefenseState* targetDefense = nullptr);
};
