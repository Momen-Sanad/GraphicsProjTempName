#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../../engine/ecs/World.hpp"
#include "../../engine/components/HealthComponent.hpp"
#include "../../engine/components/CombatComponent.hpp"
#include "../../engine/components/HurtboxComponent.hpp"

class Player;

class Enemy {
public:
    Enemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial);

    Entity* entity() const { return root; }
    void setPosition(const glm::vec3& p);
    glm::vec3 getPosition() const;
    
    // AI behavior
    void setTarget(Entity* target) { targetEntity = target; }
    void update(float deltaTime);
    
    // Combat
    bool isAttacking() const { return attackTimer > 0.0f; }
    CombatComponent& getCombat() { return combat; }
    
    // Settings
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setAttackRange(float range) { attackRange = range; }
    void setDetectionRange(float range) { detectionRange = range; }

private:
    World& worldRef;
    Entity* root = nullptr;
    Entity* body = nullptr;
    Entity* targetEntity = nullptr;
    
    // AI params
    float moveSpeed = 2.5f;
    float attackRange = 2.0f;
    float detectionRange = 15.0f;
    
    // Combat
    CombatComponent combat;
    float attackDuration = 0.3f;
    float attackCooldown = 1.0f;
    float attackTimer = 0.0f;
    float attackCooldownTimer = 0.0f;
};

std::unique_ptr<Enemy> CreateEnemy(World& world,
                                   Entity* parent,
                                   MeshRenderer* bodyMesh,
                                   Material* bodyMaterial);
