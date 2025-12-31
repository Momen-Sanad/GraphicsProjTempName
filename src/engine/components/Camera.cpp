#include "CameraUtils.hpp"
#include <glm/gtc/matrix_transform.hpp>

static float safe_aspect_ratio(glm::vec2 viewportSize) {
    if (viewportSize.y <= 0.0f) {
        return 1.0f;
    }
    return viewportSize.x / viewportSize.y;
}

glm::mat4 camera_view_matrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.position + camera.direction, camera.up);
}

glm::mat4 camera_projection_matrix(const Camera& camera, glm::vec2 viewportSize) {
    return glm::perspective(camera.fov, safe_aspect_ratio(viewportSize), camera.near, camera.far);
}

glm::mat4 camera_view_projection_matrix(const Camera& camera, glm::vec2 viewportSize) {
    return camera_projection_matrix(camera, viewportSize) * camera_view_matrix(camera);
}
