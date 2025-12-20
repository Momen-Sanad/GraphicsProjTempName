#pragma once
#include <glm/glm.hpp>

struct HurtboxComponent {
    glm::vec3 halfExtents = {0.5f, 0.5f, 0.5f};
    glm::vec3 localOffset = {0,0,0};
    bool enabled = true;
};
