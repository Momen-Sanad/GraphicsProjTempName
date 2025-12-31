#pragma once

#include <glm/glm.hpp>

enum class LightType {
    Directional,
    Point,
    Spot
};

struct Light {
    LightType type = LightType::Point;
    glm::vec3 color{1.0f, 1.0f, 1.0f};
    float intensity = 1.0f;
    float range = 10.0f;
    float innerCone = 0.2617994f; // 15 degrees in radians
    float outerCone = 0.4363323f; // 25 degrees in radians
};
