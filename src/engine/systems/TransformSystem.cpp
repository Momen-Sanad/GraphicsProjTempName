#include "TransformSystem.hpp"
#include "../ecs/World.hpp"

TransformSystem::TransformSystem(World& world)
    : m_world(world) {}

void TransformSystem::update(float) {
    // Placeholder for hierarchy/world-matrix updates.
}
