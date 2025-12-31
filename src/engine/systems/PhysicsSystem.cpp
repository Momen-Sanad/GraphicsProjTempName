#include "PhysicsSystem.hpp"
#include "../ecs/World.hpp"

namespace gproj::physics {

PhysicsSystem::PhysicsSystem(World& world, PhysicsBackend& backend)
    : m_world(world), m_backend(backend) {}

void PhysicsSystem::initialize() {
    m_backend.initialize();
}

void PhysicsSystem::shutdown() {
    m_backend.shutdown();
}

void PhysicsSystem::fixedUpdate(float dtSeconds) {
    m_backend.stepSimulation(dtSeconds);
}

} // namespace gproj::physics
