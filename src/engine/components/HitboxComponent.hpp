#pragma once
#include <glm/glm.hpp>

enum class CollisionLayer : uint32_t {
    Player = 1<<0,
    Enemy  = 1<<1,
    Terrain= 1<<2,
    Projectile = 1<<3,
    Collectible = 1<<4,
};

struct HitboxComponent {
    // local-space AABB centered around origin: half extents
    glm::vec3 halfExtents = {0.5f, 0.5f, 0.5f};
    glm::vec3 localOffset = {0.0f, 0.0f, 0.0f};
    CollisionLayer layer = CollisionLayer::Player;
    uint32_t mask = static_cast<uint32_t>(CollisionLayer::Enemy) | static_cast<uint32_t>(CollisionLayer::Terrain);
    bool enabled = true;

    void get_world_aabb(const glm::vec3& ownerPos,
                        const glm::vec3& forward,
                        glm::vec3& outMin,
                        glm::vec3& outMax) const;
};


/*
Damage flow example

1. PhysicsCollisionSystem detects overlap Hitbox H (owner = player) vs Hurtbox T (owner = enemy).

2. It emits CollisionEvent{hitEntity=H_owner, hurtEntity=T_owner, penetration}.

3. CombatSystem listens, checks if hit should apply (owner not same team, invuln, blocking), computes damage, and emits DamageEvent.

4. HealthSystem subtracts HP, starts invulnerability (invulnTimer = invulnDuration), and signals DeathEvent if HP<=0.

5. SpawnSystem handles respawn or ragdoll/death animation if present.

*/
