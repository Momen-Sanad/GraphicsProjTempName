#include "Camera.hpp"

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 Camera::get_projection_matrix(glm::vec2 viewport_size) const {
    float aspect = viewport_size.x / viewport_size.y;
    return glm::perspective(fov, aspect, near, far);
}

glm::mat4 Camera::get_view_projection_matrix(glm::vec2 viewport_size) const {
    return get_projection_matrix(viewport_size) * get_view_matrix();
}
