#include "PhysicsCollisionSystem.hpp"

#include "../ecs/Registry.hpp"

#include <limits>

bool PhysicsCollisionSystem::computeMeshBounds(const Mesh& mesh, MeshBounds& outBounds) {
    const auto positions = mesh.get_positions();
    if (positions.empty()) return false;

    glm::vec3 minPos = positions.front();
    glm::vec3 maxPos = positions.front();

    for (const auto& pos : positions) {
        minPos = glm::min(minPos, pos);
        maxPos = glm::max(maxPos, pos);
    }

    outBounds.center = (minPos + maxPos) * 0.5f;
    outBounds.halfExtents = (maxPos - minPos) * 0.5f;
    return true;
}

AABB PhysicsCollisionSystem::computeWorldAABB(
    const engine::ecs::Transform& transform,
    const engine::ecs::ColliderData& collider)
{
    glm::vec3 center = collider.localOffset;
    glm::vec3 extents = collider.halfExtents;

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

    glm::mat4 world = transform.worldMatrix;
    glm::vec3 minPos(std::numeric_limits<float>::infinity());
    glm::vec3 maxPos(-std::numeric_limits<float>::infinity());
    for (auto& corner : corners) {
        corner = glm::vec3(world * glm::vec4(corner, 1.0f));
        minPos = glm::min(minPos, corner);
        maxPos = glm::max(maxPos, corner);
    }

    return {minPos, maxPos};
}

bool PhysicsCollisionSystem::intersects(
    const engine::ecs::Transform& aTransform,
    const engine::ecs::ColliderData& aCollider,
    const engine::ecs::Transform& bTransform,
    const engine::ecs::ColliderData& bCollider)
{
    AABB a = computeWorldAABB(aTransform, aCollider);
    AABB b = computeWorldAABB(bTransform, bCollider);

    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool PhysicsCollisionSystem::resolveStaticCollision(
    engine::ecs::Registry& registry,
    engine::ecs::EntityId mover,
    engine::ecs::EntityId obstacle)
{
    auto* moverTransform = registry.get<engine::ecs::Transform>(mover);
    auto* moverCollider = registry.get<engine::ecs::ColliderData>(mover);
    auto* obstacleTransform = registry.get<engine::ecs::Transform>(obstacle);
    auto* obstacleCollider = registry.get<engine::ecs::ColliderData>(obstacle);

    if (!moverTransform || !moverCollider || !obstacleTransform || !obstacleCollider) {
        return false;
    }

    if (!intersects(*moverTransform, *moverCollider, *obstacleTransform, *obstacleCollider)) {
        return false;
    }

    AABB a = computeWorldAABB(*moverTransform, *moverCollider);
    AABB b = computeWorldAABB(*obstacleTransform, *obstacleCollider);

    glm::vec3 aCenter = (a.min + a.max) * 0.5f;
    glm::vec3 bCenter = (b.min + b.max) * 0.5f;
    glm::vec3 aExtents = (a.max - a.min) * 0.5f;
    glm::vec3 bExtents = (b.max - b.min) * 0.5f;

    glm::vec3 delta = aCenter - bCenter;
    glm::vec3 overlap = aExtents + bExtents - glm::abs(delta);

    if (overlap.x <= 0.0f || overlap.y <= 0.0f || overlap.z <= 0.0f) {
        return false;
    }

    glm::vec3 correction(0.0f);
    if (overlap.x < overlap.y && overlap.x < overlap.z) {
        correction.x = (delta.x < 0.0f) ? -overlap.x : overlap.x;
    } else if (overlap.y < overlap.z) {
        correction.y = (delta.y < 0.0f) ? -overlap.y : overlap.y;
    } else {
        correction.z = (delta.z < 0.0f) ? -overlap.z : overlap.z;
    }

    moverTransform->position += correction;
    moverTransform->dirty = true;
    return true;
}
