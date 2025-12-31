#pragma once
#include <glm/glm.hpp>

struct MovementComponent {
    glm::vec3 velocity = {0,0,0};
    glm::vec3 acceleration = {0,0,0};
    float mass = 1.0f;
    float maxSpeed = 8.0f;
    bool grounded = false;
};
