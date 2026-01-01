#pragma once
#include <glm/glm.hpp>

struct HurtboxComponent {
    glm::vec3 halfExtents = {0.5f, 0.5f, 0.5f};
    glm::vec3 localOffset = {0,0,0};
    bool enabled = true;

    void get_world_aabb(const glm::vec3& ownerPos,
                        glm::vec3& outMin,
                        glm::vec3& outMax) const;
};
