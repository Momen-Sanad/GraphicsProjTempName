#include "Camera.hpp"

// ------------------------------------------------------
// Returns the view matrix for the camera
// This matrix transforms world coordinates into camera coordinates (view space).
// ------------------------------------------------------
glm::mat4 Camera::get_view_matrix() const {
    // Using glm's lookAt function, which generates a view matrix
    // The view matrix is defined by the camera's position, direction, and up vector
    return glm::lookAt(position, position + direction, up);
}

// ------------------------------------------------------
// Returns the projection matrix for the camera
// This matrix is used to convert camera space coordinates into normalized device coordinates (NDC).
// The aspect ratio of the viewport (width/height) is taken into account here.
// ------------------------------------------------------
glm::mat4 Camera::get_projection_matrix(glm::vec2 viewport_size) const {
    float aspect = viewport_size.x / viewport_size.y;  // Aspect ratio of the viewport (width / height)
    // Using glm's perspective function, which generates a perspective projection matrix
    return glm::perspective(fov, aspect, near, far);
}

// ------------------------------------------------------
// Returns the combined view-projection matrix
// This is the result of multiplying the view matrix with the projection matrix.
// It is often used directly in shaders to transform world coordinates into NDC in one step.
// ------------------------------------------------------
glm::mat4 Camera::get_view_projection_matrix(glm::vec2 viewport_size) const {
    // Multiply the projection matrix by the view matrix for a combined transformation
    return get_projection_matrix(viewport_size) * get_view_matrix();
}
