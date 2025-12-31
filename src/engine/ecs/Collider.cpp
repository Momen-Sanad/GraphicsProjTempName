#include "Collider.hpp"

#include "Entity.hpp"

#include <limits>

Collider::Collider(const glm::vec3& extents, const glm::vec3& offset)
    : halfExtents(extents), localOffset(offset) {}

AABB Collider::getWorldAABB() const {
    glm::vec3 center = localOffset;
    glm::vec3 extents = halfExtents;

    glm::vec3 corners[8] = {
        center + glm::vec3(-extents.x, -extents.y, -extents.z),
        center + glm::vec3( extents.x, -extents.y, -extents.z),
        center + glm::vec3(-extents.x,  extents.y, -extents.z),
        center + glm::vec3( extents.x,  extents.y, -extents.z),
        center + glm::vec3(-extents.x, -extents.y,  extents.z),
        center + glm::vec3( extents.x, -extents.y,  extents.z),
        center + glm::vec3(-extents.x,  extents.y,  extents.z),
        center + glm::vec3( extents.x,  extents.y,  extents.z),
    };

    if (parent) {
        glm::mat4 world = parent->getWorldMatrix();
        for (auto& corner : corners) {
            corner = glm::vec3(world * glm::vec4(corner, 1.0f));
        }
    }

    glm::vec3 minPos(std::numeric_limits<float>::infinity());
    glm::vec3 maxPos(-std::numeric_limits<float>::infinity());
    for (const auto& corner : corners) {
        minPos = glm::min(minPos, corner);
        maxPos = glm::max(maxPos, corner);
    }

    return {minPos, maxPos};
}

bool Collider::intersects(const Collider& other) const {
    if (!enabled || !other.enabled) return false;

    AABB a = getWorldAABB();
    AABB b = other.getWorldAABB();

    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}
