#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "../components/Camera.hpp"

class CameraController {
public:
    float mouse_sensitivity      = 0.01f;
    float normal_movement_speed  = 1.0f;
    float run_movement_speed     = 5.0f;

    CameraController() = default;

    void setup(GLFWwindow* window, Camera* camera);
    void update(float delta_time);

private:
    GLFWwindow* window = nullptr;
    Camera* camera     = nullptr;

    bool enabled = false;
    glm::dvec2 last_mouse_position = {0, 0};
    float pitch = 0.0f;
    float yaw   = 0.0f;
};

#endif
