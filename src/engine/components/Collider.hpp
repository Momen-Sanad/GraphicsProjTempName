#pragma once

#include <glm/glm.hpp>

struct Collider {
    glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};
    glm::vec3 localOffset{0.0f, 0.0f, 0.0f};
    bool enabled = true;
};
