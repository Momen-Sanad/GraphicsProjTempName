#pragma once

#include <unordered_map>


#include "PhysicsBackend.hpp"

class EntityManager;
class Entity;

namespace gproj::physics {

class PhysicsSystem {
    
private:

    EntityManager&   m_entityManager;
    PhysicsBackend& m_backend;

    // Map each entity to its physics body
    std::unordered_map<Entity*, PhysicsBodyId> m_entityToBody;

    void syncEntityTransformToPhysics(Entity* entity, PhysicsBodyId bodyId);
    void syncPhysicsToEntityTransform(Entity* entity, PhysicsBodyId bodyId);

public:
    
    PhysicsSystem(EntityManager& entityManager, PhysicsBackend& backend);

    void initialize();
    void shutdown();

    // Called every frame with delta time
    void update(float dtSeconds);

    // Attach a rigid body to an Entity*
    PhysicsBodyId createRigidBodyForEntity(Entity* entity, const RigidBodyDesc& desc);
    
    void removeRigidBodyForEntity(Entity* entity);

    bool hasRigidBody(Entity* entity) const;
    void onEntityDestroyed(Entity* entity);

    // sync helpers
    void syncEntityToPhysics(Entity* entity);
    void syncPhysicsToEntity(Entity* entity);

<<<<<<< HEAD
    // void syncEntityTransformToPhysics(Entity* entity, PhysicsBodyId bodyId);
    // void syncPhysicsToEntityTransform(Entity* entity, PhysicsBodyId bodyId);

=======
>>>>>>> temp
};

}