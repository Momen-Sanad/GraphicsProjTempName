#include "CombatComponent.hpp"

static bool aabb_intersects(const glm::vec3& a_min,
                            const glm::vec3& a_max,
                            const glm::vec3& b_min,
                            const glm::vec3& b_max) {
    return (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
           (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
           (a_min.z <= b_max.z && a_max.z >= b_min.z);
}

void CombatComponent::update_attack_state(bool isAttacking) {
    if (isAttacking && !wasAttacking) {
        hitThisSwing = false;
    }
    if (!isAttacking) {
        hitThisSwing = false;
    }
    wasAttacking = isAttacking;
}

bool CombatComponent::resolve_attack(bool isAttacking,
                                     const glm::vec3& attackerPos,
                                     const glm::vec3& attackerForward,
                                     const HurtboxComponent& targetHurtbox,
                                     const glm::vec3& targetPos,
                                     HealthComponent& targetHealth,
                                     const DefenseState* targetDefense) {
    update_attack_state(isAttacking);

    if (!isAttacking || hitThisSwing || !hitbox.enabled || !targetHurtbox.enabled) {
        return false;
    }

    glm::vec3 hitMin, hitMax;
    glm::vec3 targetMin, targetMax;
    hitbox.get_world_aabb(attackerPos, attackerForward, hitMin, hitMax);
    targetHurtbox.get_world_aabb(targetPos, targetMin, targetMax);

    if (!aabb_intersects(hitMin, hitMax, targetMin, targetMax)) {
        return false;
    }

    // Calculate actual damage based on defense state
    int actualDamage = damage;
    
    if (targetDefense) {
        // Blocking = no damage
        if (targetDefense->blocking) {
            hitThisSwing = true;
            return false;  // Attack was blocked
        }
        
        // Dodge within window = half damage
        if (targetDefense->dodging && targetDefense->dodgeTimer <= targetDefense->dodgeWindow) {
            actualDamage = damage / 2;
        }
    }

    if (targetHealth.apply_damage(actualDamage)) {
        hitThisSwing = true;
        return true;
    }

    return false;
}
