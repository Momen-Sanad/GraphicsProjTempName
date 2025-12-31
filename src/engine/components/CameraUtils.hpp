#pragma once

#include "Camera.hpp"
#include <glm/glm.hpp>

glm::mat4 camera_view_matrix(const Camera& camera);
glm::mat4 camera_projection_matrix(const Camera& camera, glm::vec2 viewportSize);
glm::mat4 camera_view_projection_matrix(const Camera& camera, glm::vec2 viewportSize);
