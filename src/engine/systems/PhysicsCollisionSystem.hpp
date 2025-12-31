#pragma once

#include <glm/glm.hpp>

#include "../components/Collider.hpp"
#include "../ecs/Entity.hpp"
#include "../gl/Mesh.hpp"

class PhysicsCollisionSystem {
public:
    struct MeshBounds {
        glm::vec3 center{0.0f, 0.0f, 0.0f};
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    };

    static bool computeMeshBounds(const Mesh& mesh, MeshBounds& outBounds);
    struct AABB {\n        glm::vec3 min{0.0f};\n        glm::vec3 max{0.0f};\n    };\n+\n+    static bool resolveStaticCollision(Entity* mover,\n+                                       const Collider& moverCollider,\n+                                       Entity* staticEntity,\n+                                       const Collider& staticCollider);
};
