#include "PhysicsSystem.hpp"
<<<<<<< HEAD
#include "engine/ecs/EntityManager.hpp"
#include "engine/ecs/Entity.hpp"
=======
#include "../ecs/EntityManager.hpp"
#include "../ecs/Entity.hpp"
>>>>>>> temp

namespace gproj::physics {

PhysicsSystem::PhysicsSystem(EntityManager& entityManager,
                             PhysicsBackend& backend)
<<<<<<< HEAD
    : m_entityManager(entityManager)
    , m_backend(backend) {}
=======
   : m_entityManager(entityManager)
   , m_backend(backend) {}
>>>>>>> temp

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
   Entity* entity,
   const RigidBodyDesc& desc) {

   if (!entity) return 0; // 0 = invalid body

   PhysicsBodyId bodyId = m_backend.createRigidBody(desc);
   m_entityToBody[entity] = bodyId;

   // Ensure entity starts with the physics transform
   syncPhysicsToEntityTransform(entity, bodyId);
   return bodyId;
}

void PhysicsSystem::removeRigidBodyForEntity(Entity* entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;

   m_backend.destroyRigidBody(it->second);
   m_entityToBody.erase(it);
}

bool PhysicsSystem::hasRigidBody(Entity* entity) const {
   return m_entityToBody.find(entity) != m_entityToBody.end();
}

void PhysicsSystem::onEntityDestroyed(Entity* entity) {
   removeRigidBodyForEntity(entity);
}

void PhysicsSystem::syncEntityToPhysics(Entity* entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;
   syncEntityTransformToPhysics(entity, it->second);
}

void PhysicsSystem::syncPhysicsToEntity(Entity* entity) {
   auto it = m_entityToBody.find(entity);
   if (it == m_entityToBody.end()) return;
   syncPhysicsToEntityTransform(entity, it->second);
}

void PhysicsSystem::syncEntityTransformToPhysics(Entity* entity,
                                                PhysicsBodyId bodyId) {
   if (!entity) return;

   PhysicsTransform pt;
   pt.position = entity->getPosition();
   pt.rotation = entity->getRotation();

   m_backend.setBodyTransform(bodyId, pt);
}

void PhysicsSystem::syncPhysicsToEntityTransform(Entity* entity,
                                                PhysicsBodyId bodyId) {
   if (!entity) return;

   PhysicsTransform pt = m_backend.getBodyTransform(bodyId);

   entity->setPosition(pt.position);
   entity->setRotation(pt.rotation);
}

}