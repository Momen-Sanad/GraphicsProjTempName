#pragma once

#include <glm/glm.hpp>

#include "../ecs/Collider.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/EntityId.hpp"
#include "../gl/Mesh.hpp"

namespace engine::ecs {
class Registry;
}

class PhysicsCollisionSystem {
public:
    struct MeshBounds {
        glm::vec3 center{0.0f, 0.0f, 0.0f};
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    };

    static bool computeMeshBounds(const Mesh& mesh, MeshBounds& outBounds);
    static AABB computeWorldAABB(const engine::ecs::Transform& transform,
                                 const engine::ecs::ColliderData& collider);
    static bool intersects(const engine::ecs::Transform& aTransform,
                           const engine::ecs::ColliderData& aCollider,
                           const engine::ecs::Transform& bTransform,
                           const engine::ecs::ColliderData& bCollider);
    static bool resolveStaticCollision(engine::ecs::Registry& registry,
                                       engine::ecs::EntityId mover,
                                       engine::ecs::EntityId obstacle);
};
