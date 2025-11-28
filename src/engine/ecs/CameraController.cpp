#include "CameraController.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

void CameraController::setup(GLFWwindow* window, Camera* camera) {
    this->window = window;
    this->camera = camera;
    enabled = false;

    glm::vec3 dir = glm::normalize(camera->direction);
    pitch = glm::asin(dir.y);
    yaw   = glm::atan(-dir.z, dir.x);
}

void CameraController::update(float dt) {
    if (!window || !camera) return;

    // Mouse button toggles control mode
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!enabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &last_mouse_position.x, &last_mouse_position.y);
            enabled = true;
        }
    } else {
        if (enabled) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            enabled = false;
        }
    }

    if (!enabled) return;

    // Mouse movement
    glm::dvec2 curr_mouse;
    glfwGetCursorPos(window, &curr_mouse.x, &curr_mouse.y);
    glm::vec2 delta_mouse = glm::vec2(curr_mouse - last_mouse_position);
    last_mouse_position = curr_mouse;

    pitch -= delta_mouse.y * mouse_sensitivity;
    yaw   -= delta_mouse.x * mouse_sensitivity;

    pitch = glm::clamp(pitch, -glm::half_pi<float>() * 0.98f, glm::half_pi<float>() * 0.98f);
    yaw   = glm::mod(yaw, glm::two_pi<float>());

    // Update direction vector
    camera->direction = glm::vec3(
        glm::cos(pitch) * glm::cos(yaw),
        glm::sin(pitch),
        glm::cos(pitch) * -glm::sin(yaw)
    );

    // Movement
    glm::vec3 right = glm::normalize(glm::cross(camera->direction, camera->up));
    glm::vec3 movement(0.0f);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement += camera->direction;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement -= camera->direction;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement += right;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement -= right;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement += camera->up;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement -= camera->up;

    if (movement != glm::vec3(0.0f)) {
        movement = glm::normalize(movement);
        float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                        ? run_movement_speed
                        : normal_movement_speed;

        camera->position += movement * speed * dt;
    }
}