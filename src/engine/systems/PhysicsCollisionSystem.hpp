#pragma once

#include <glm/glm.hpp>

#include "../ecs/Collider.hpp"
#include "../ecs/Entity.hpp"
#include "../gl/Mesh.hpp"

class PhysicsCollisionSystem {
public:
    struct MeshBounds {
        glm::vec3 center{0.0f, 0.0f, 0.0f};
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    };

    static bool computeMeshBounds(const Mesh& mesh, MeshBounds& outBounds);
    static bool resolveStaticCollision(Entity* mover,
                                       const Collider& moverCollider,
                                       const Collider& staticCollider);
};
