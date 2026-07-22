#include "HurtboxComponent.hpp"

void HurtboxComponent::get_world_aabb(const glm::vec3& ownerPos,
                                      glm::vec3& outMin,
                                      glm::vec3& outMax) const {
    if (!enabled) {
        outMin = ownerPos;
        outMax = ownerPos;
        return;
    }

    glm::vec3 center = ownerPos + localOffset;
    outMin = center - halfExtents;
    outMax = center + halfExtents;
}
