#include "TransformSystem.hpp"

#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace {
uint64_t entity_key(engine::ecs::EntityId id) {
    return (static_cast<uint64_t>(id.generation) << 32u) | id.index;
}

glm::mat4 resolve_world_matrix(
    engine::ecs::Registry& registry,
    engine::ecs::EntityId id,
    std::unordered_map<uint64_t, uint8_t>& state)
{
    auto* transform = registry.get<engine::ecs::Transform>(id);
    if (!transform) {
        return glm::mat4(1.0f);
    }

    const uint64_t key = entity_key(id);
    const auto existing = state.find(key);
    if (existing != state.end()) {
        if (existing->second == 2) {
            return transform->worldMatrix;
        }
        transform->worldMatrix = transform->localMatrix();
        transform->dirty = false;
        existing->second = 2;
        return transform->worldMatrix;
    }

    state[key] = 1;

    glm::mat4 parentWorld(1.0f);
    const auto* hierarchy = registry.get<engine::ecs::Hierarchy>(id);
    if (hierarchy && hierarchy->parent.valid() && registry.isAlive(hierarchy->parent)) {
        parentWorld = resolve_world_matrix(registry, hierarchy->parent, state);
    }

    transform->worldMatrix = parentWorld * transform->localMatrix();
    transform->dirty = false;
    state[key] = 2;
    return transform->worldMatrix;
}
} // namespace

void TransformSystem::update(engine::ecs::Registry& registry, float) {
    updateWorldTransforms(registry);
}

void TransformSystem::updateWorldTransforms(engine::ecs::Registry& registry) {
    std::vector<engine::ecs::EntityId> entities = registry.entitiesWith<engine::ecs::Transform>();
    std::unordered_map<uint64_t, uint8_t> state;
    state.reserve(entities.size());

    for (engine::ecs::EntityId id : entities) {
        resolve_world_matrix(registry, id, state);
    }
}
