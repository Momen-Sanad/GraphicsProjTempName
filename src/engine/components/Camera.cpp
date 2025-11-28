#include "Camera.hpp"

// Constructor to initialize default camera values
Camera::Camera() :
    position(0.0f, 0.0f, 3.0f),
    direction(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    fov(glm::radians(60.0f)), // Field of view set to 60 degrees
    near(0.1f),                // Near clipping plane
    far(100.0f)                // Far clipping plane
{}

// Get the view matrix (camera position and orientation)
glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(position, position + direction, up);
}

// Get the projection matrix (used for perspective projection)
glm::mat4 Camera::get_projection_matrix(float aspect_ratio) const {
    return glm::perspective(fov, aspect_ratio, near, far);
}

// Combine both view and projection matrices for easy use in the main loop
glm::mat4 Camera::get_view_projection_matrix(float aspect_ratio) const {
    return get_projection_matrix(aspect_ratio) * get_view_matrix();
}
