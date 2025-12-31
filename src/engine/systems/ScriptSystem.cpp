#include "ScriptSystem.hpp"
#include "../ecs/World.hpp"

ScriptSystem::ScriptSystem(World& world)
    : m_world(world) {}

void ScriptSystem::update(float) {
    // Placeholder for running script callbacks.
}
