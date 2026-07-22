#pragma once

#include <glm/glm.hpp>

struct HealthComponent;
struct HurtboxComponent;
namespace engine::ecs {
struct Transform;
}

class HealthBarSystem {
public:
    static void draw_over_entity(const HealthComponent& health,
                                 const HurtboxComponent& hurtbox,
                                 const engine::ecs::Transform& transform,
                                 const glm::mat4& viewProj,
                                 int viewportWidth,
                                 int viewportHeight);
};
