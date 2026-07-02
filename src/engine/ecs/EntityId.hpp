#pragma once

#include <cstdint>
#include <functional>

namespace engine::ecs {

struct EntityId {
    uint32_t index = UINT32_MAX;
    uint32_t generation = 0;

    constexpr bool valid() const { return index != UINT32_MAX; }

    friend constexpr bool operator==(EntityId lhs, EntityId rhs) {
        return lhs.index == rhs.index && lhs.generation == rhs.generation;
    }

    friend constexpr bool operator!=(EntityId lhs, EntityId rhs) {
        return !(lhs == rhs);
    }
};

inline constexpr EntityId InvalidEntity{};

struct EntityIdHash {
    size_t operator()(EntityId id) const noexcept {
        return (static_cast<size_t>(id.generation) << 32) ^ id.index;
    }
};

} // namespace engine::ecs

using EntityId = engine::ecs::EntityId;
