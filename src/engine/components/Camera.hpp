#pragma once
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// The Camera class represents a 3D camera, typically used for rendering scenes in 3D space.
class Camera {
public:
    // Position of the camera in 3D space, default is (0, 0, 3)
    glm::vec3 position = {0.0f, 0.0f, 3.0f};

    // Direction the camera is facing, default is along the negative Z-axis
    glm::vec3 direction = {0.0f, 0.0f, -1.0f};

    // Up vector, defining the "top" direction, default is along the positive Y-axis
    glm::vec3 up        = {0.0f, 1.0f, 0.0f};

    // Field of view (FOV) in radians, default is 60 degrees
    float fov  = glm::radians(60.0f);

    // Near clipping plane distance, default is 0.01 units
    float near = 0.01f;

    // Far clipping plane distance, default is 100 units
    float far  = 100.0f;

    // Default constructor (not explicitly required, uses default member initialization)
    Camera() = default;

    // Function to get the camera's view matrix (transform from world space to camera space)
    glm::mat4 get_view_matrix() const;

    // Function to get the camera's projection matrix (from camera space to normalized device coordinates)
    glm::mat4 get_projection_matrix(glm::vec2 viewport_size) const;

    // Function to get the combined view-projection matrix (multiplication of view and projection matrices)
    glm::mat4 get_view_projection_matrix(glm::vec2 viewport_size) const;
};

#endif