#include "PhysicsCollisionSystem.hpp"

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

bool PhysicsCollisionSystem::resolveStaticCollision(Entity* mover,
                                                    const Collider& moverCollider,
                                                    const Collider& staticCollider) {
    if (!mover) return false;
    if (!moverCollider.intersects(staticCollider)) return false;

    AABB a = moverCollider.getWorldAABB();
    AABB b = staticCollider.getWorldAABB();

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

    mover->setPosition(mover->getPosition() + correction);
    return true;
}
