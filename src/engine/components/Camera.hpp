#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position = {0.0f, 0.0f, 3.0f};
    glm::vec3 direction = {0.0f, 0.0f, -1.0f};
    glm::vec3 up        = {0.0f, 1.0f, 0.0f};

    float fov  = glm::radians(60.0f);
    float near = 0.01f;
    float far  = 100.0f;

    Camera() = default;

    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix(glm::vec2 viewport_size) const;
    glm::mat4 get_view_projection_matrix(glm::vec2 viewport_size) const;
};

#endif
