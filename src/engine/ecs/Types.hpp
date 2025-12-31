#pragma once

#include <cstdint>
#include <limits>

using EntityId = std::uint32_t;
constexpr EntityId kInvalidEntity = std::numeric_limits<EntityId>::max();
