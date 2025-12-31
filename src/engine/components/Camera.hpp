#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 3.0f};
    glm::vec3 direction{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    float fov = glm::radians(60.0f);
    float near = 0.01f;
    float far = 100.0f;
};
