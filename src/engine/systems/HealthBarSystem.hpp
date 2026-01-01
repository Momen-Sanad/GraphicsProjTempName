#pragma once

#include <glm/glm.hpp>

struct HealthComponent;
struct HurtboxComponent;
class Entity;

class HealthBarSystem {
public:
    static void draw_over_entity(const HealthComponent& health,
                                 const HurtboxComponent& hurtbox,
                                 const Entity& entity,
                                 const glm::mat4& viewProj,
                                 int viewportWidth,
                                 int viewportHeight);
};
