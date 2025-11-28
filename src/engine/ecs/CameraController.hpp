#pragma once
#include "../components/Camera.hpp"
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>

class CameraController {
public:
    GLFWwindow* window;
    Camera* camera;

    float mouse_sensitivity;
    float normal_movement_speed;
    float run_movement_speed;

    float last_mouse_x = 0.0f;
    float last_mouse_y = 0.0f;

    CameraController(GLFWwindow* window, Camera* camera);
    void update(float deltaTime);

private:
    bool enabled;
    float pitch, yaw;
    glm::dvec2 last_mouse_position;
    void update_camera_direction();
};
