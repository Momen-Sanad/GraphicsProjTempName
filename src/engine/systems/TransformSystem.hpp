#pragma once

#include "../ecs/System.hpp"

namespace engine::ecs {
class Registry;
}

class TransformSystem final : public engine::ecs::System {
public:
    void update(engine::ecs::Registry& registry, float deltaTime) override;

    static void updateWorldTransforms(engine::ecs::Registry& registry);
};
