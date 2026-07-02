#include "PhysicsSystem.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

namespace gproj::physics {

PhysicsSystem::PhysicsSystem(engine::ecs::Registry& registry,
                             PhysicsBackend& backend)
   : m_registry(registry)
   , m_backend(backend) {}

void PhysicsSystem::initialize() {
   m_backend.initialize();
}

void PhysicsSystem::shutdown() {
   for (auto& [entity, bodyId] : m_entityToBody) {
       m_backend.destroyRigidBody(bodyId);
   }
   m_entityToBody.clear();
   m_backend.shutdown();
}

void PhysicsSystem::update(float dtSeconds) {
   // Step the physics simulation
   m_backend.stepSimulation(dtSeconds);

   // Pull updated transforms back into entities
   for (auto& [entity, bodyId] : m_entityToBody) {
       syncPhysicsToEntityTransform(entity, bodyId);
   }
}

PhysicsBodyId PhysicsSystem::createRigidBodyForEntity(
   engine::ecs::EntityId entity,
   const RigidBodyDesc& desc) {

   if (!m_registry.isAlive(entity)) return 0; // 0 = invalid body

   PhysicsBodyId bodyId = m_backend.createRigidBody(desc);
   m_entityToBody[entity] = bodyId;

   // Ensure entity starts with the physics transform
   syncPhysicsToEntityTransform(entity, bodyId);
   return bodyId;
}

void PhysicsSystem::removeRigidBodyForEntity(engine::ecs::EntityId entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;

   m_backend.destroyRigidBody(it->second);
   m_entityToBody.erase(it);
}

bool PhysicsSystem::hasRigidBody(engine::ecs::EntityId entity) const {
   return m_entityToBody.find(entity) != m_entityToBody.end();
}

void PhysicsSystem::onEntityDestroyed(engine::ecs::EntityId entity) {
   removeRigidBodyForEntity(entity);
}

void PhysicsSystem::syncEntityToPhysics(engine::ecs::EntityId entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;
   syncEntityTransformToPhysics(entity, it->second);
}

void PhysicsSystem::syncPhysicsToEntity(engine::ecs::EntityId entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;
   syncPhysicsToEntityTransform(entity, it->second);
}

void PhysicsSystem::syncEntityTransformToPhysics(engine::ecs::EntityId entity,
                                                PhysicsBodyId bodyId) {
   auto* transform = m_registry.get<engine::ecs::Transform>(entity);
   if (!transform) return;

   PhysicsTransform pt;
   pt.position = transform->position;
   pt.rotation = transform->rotation;

   m_backend.setBodyTransform(bodyId, pt);
}

void PhysicsSystem::syncPhysicsToEntityTransform(engine::ecs::EntityId entity,
                                                PhysicsBodyId bodyId) {
   auto* transform = m_registry.get<engine::ecs::Transform>(entity);
   if (!transform) return;

   PhysicsTransform pt = m_backend.getBodyTransform(bodyId);

   transform->position = pt.position;
   transform->rotation = pt.rotation;
   transform->dirty = true;
}

}
