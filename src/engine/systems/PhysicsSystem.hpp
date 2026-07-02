#pragma once

#include <unordered_map>

#include "../ecs/EntityId.hpp"
#include "PhysicsBackend.hpp"

namespace engine::ecs {
class Registry;
}

namespace gproj::physics {

class PhysicsSystem {
    
private:

    engine::ecs::Registry& m_registry;
    PhysicsBackend& m_backend;

    std::unordered_map<engine::ecs::EntityId, PhysicsBodyId, engine::ecs::EntityIdHash> m_entityToBody;

    void syncEntityTransformToPhysics(engine::ecs::EntityId entity, PhysicsBodyId bodyId);
    void syncPhysicsToEntityTransform(engine::ecs::EntityId entity, PhysicsBodyId bodyId);

public:
    
    PhysicsSystem(engine::ecs::Registry& registry, PhysicsBackend& backend);

    void initialize();
    void shutdown();

    // Called every frame with delta time
    void update(float dtSeconds);

    PhysicsBodyId createRigidBodyForEntity(engine::ecs::EntityId entity, const RigidBodyDesc& desc);
    
    void removeRigidBodyForEntity(engine::ecs::EntityId entity);

    bool hasRigidBody(engine::ecs::EntityId entity) const;
    void onEntityDestroyed(engine::ecs::EntityId entity);

    // sync helpers
    void syncEntityToPhysics(engine::ecs::EntityId entity);
    void syncPhysicsToEntity(engine::ecs::EntityId entity);

};

}
