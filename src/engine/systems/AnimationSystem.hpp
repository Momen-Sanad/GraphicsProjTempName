#pragma once

#include "../ecs/EntityId.hpp"
#include "../ecs/System.hpp"

namespace engine::ecs {
class Registry;
}

class AnimationSystem final : public engine::ecs::System {
public:
    void update(engine::ecs::Registry& registry, float deltaTime) override;

    static void play(
        engine::ecs::Registry& registry,
        engine::ecs::EntityId entity,
        int animationIndex,
        bool loop = true,
        float speed = 1.0f);
};
