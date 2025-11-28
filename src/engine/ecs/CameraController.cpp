#include "CameraController.hpp"

CameraController::CameraController(GLFWwindow* window, Camera* camera)
    : window(window), camera(camera), mouse_sensitivity(0.01f),
      normal_movement_speed(1.0f), run_movement_speed(5.0f), enabled(false), pitch(0.0f), yaw(0.0f) {}

void CameraController::update(float deltaTime) {
    // Handle mouse look
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

    if (!enabled) return; // If mouse is not captured, we skip the update.

    // Calculate mouse movement delta
    glm::dvec2 curr_mouse_position;
    glfwGetCursorPos(window, &curr_mouse_position.x, &curr_mouse_position.y);
    glm::vec2 delta_mouse_position = glm::vec2(curr_mouse_position - last_mouse_position);
    last_mouse_position = curr_mouse_position;

    // Update pitch and yaw based on mouse movement
    pitch -= delta_mouse_position.y * mouse_sensitivity;
    yaw -= delta_mouse_position.x * mouse_sensitivity;

    // Clamp pitch to prevent flipping
    pitch = glm::clamp(pitch, -glm::pi<float>() * 0.49f, glm::pi<float>() * 0.49f);

    // Update camera direction based on pitch and yaw
    update_camera_direction();

    // Handle keyboard movement (WASD + Shift for running)
    glm::vec3 move_dir(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) move_dir += camera->direction;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) move_dir -= camera->direction;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) move_dir -= glm::normalize(glm::cross(camera->direction, camera->up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) move_dir += glm::normalize(glm::cross(camera->direction, camera->up));

    // Adjust speed if Shift is held (run speed)
    float movement_speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? run_movement_speed : normal_movement_speed;

    if (glm::length(move_dir) > 0.0f) {
        move_dir = glm::normalize(move_dir);
        camera->position += move_dir * movement_speed * deltaTime;
    }

    // Optional: lock the cursor position (for seamless camera control)
    glfwSetCursorPos(window, last_mouse_x, last_mouse_y);
}

void CameraController::update_camera_direction() {
    // Recompute the camera's direction based on the pitch and yaw
    camera->direction = glm::vec3(
        glm::cos(pitch) * glm::cos(yaw),
        glm::sin(pitch),
        glm::cos(pitch) * -glm::sin(yaw)
    );
}
