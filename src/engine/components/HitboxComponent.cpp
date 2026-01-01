#include "HitboxComponent.hpp"

void HitboxComponent::get_world_aabb(const glm::vec3& ownerPos,
                                     const glm::vec3& forward,
                                     glm::vec3& outMin,
                                     glm::vec3& outMax) const {
    if (!enabled) {
        outMin = ownerPos;
        outMax = ownerPos;
        return;
    }

    glm::vec3 fwd = forward;
    if (glm::length(fwd) < 0.0001f) {
        fwd = glm::vec3(0.0f, 0.0f, 1.0f);
    } else {
        fwd = glm::normalize(fwd);
    }

    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::cross(up, fwd);
    if (glm::length(right) < 0.0001f) {
        right = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
        right = glm::normalize(right);
    }

    glm::vec3 center = ownerPos
        + right * localOffset.x
        + up * localOffset.y
        + fwd * localOffset.z;

    outMin = center - halfExtents;
    outMax = center + halfExtents;
}
