#pragma once
#ifndef CAMERA
#define CAMERA
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    float fov;
    float near, far;

    Camera();
    glm::mat4 get_view_matrix() const;
    glm::mat4 get_projection_matrix(float aspect_ratio) const;
    glm::mat4 get_view_projection_matrix(float aspect_ratio) const;
};
#endif