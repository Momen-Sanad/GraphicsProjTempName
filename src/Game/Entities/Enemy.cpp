#include "Enemy.hpp"
#include <glm/gtc/quaternion.hpp>
#include <iostream>

Enemy::Enemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial)
    : worldRef(world) {
    root = worldRef.createEntityWithParams(parent);
    body = worldRef.createEntityWithParams(
        root,
        {0.0f, 1.0f, 0.0f},
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        {1.0f, 2.0f, 1.0f},
        bodyMesh,
        bodyMaterial
    );
    
    // Setup combat
    combat.damage = 15;
    combat.hitbox.halfExtents = {0.5f, 0.8f, 0.5f};
    combat.hitbox.localOffset = {0.0f, 1.0f, 0.8f};
}

void Enemy::setPosition(const glm::vec3& p) {
    if (root) {
        root->setPosition(p);
    }
}

glm::vec3 Enemy::getPosition() const {
    return root ? root->getPosition() : glm::vec3(0.0f);
}

void Enemy::update(float deltaTime) {
    if (!root || !targetEntity) return;
    
    // Update attack timers
    if (attackTimer > 0.0f) {
        attackTimer -= deltaTime;
    }
    if (attackCooldownTimer > 0.0f) {
        attackCooldownTimer -= deltaTime;
    }
    
    glm::vec3 myPos = root->getPosition();
    glm::vec3 targetPos = targetEntity->getPosition();
    glm::vec3 toTarget = targetPos - myPos;
    toTarget.y = 0.0f;  // Ignore vertical
    
    float distance = glm::length(toTarget);
    
    // Only act if within detection range
    if (distance > detectionRange) return;
    
    // Face the target
    if (distance > 0.1f) {
        glm::vec3 dir = glm::normalize(toTarget);
        float angle = atan2(dir.x, dir.z);
        root->setRotation(glm::angleAxis(angle, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    
    // Move towards target if not in attack range
    if (distance > attackRange) {
        glm::vec3 moveDir = glm::normalize(toTarget);
        glm::vec3 newPos = myPos + moveDir * moveSpeed * deltaTime;
        root->setPosition(newPos);
    }
    // Attack if in range and cooldown ready
    else if (attackCooldownTimer <= 0.0f) {
        attackTimer = attackDuration;
        attackCooldownTimer = attackCooldown;
    }
}

std::unique_ptr<Enemy> CreateEnemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial) {
    return std::make_unique<Enemy>(world, parent, bodyMesh, bodyMaterial);
}
