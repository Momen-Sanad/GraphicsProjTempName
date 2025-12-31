#include "CameraController.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>

void CameraController::setup(GLFWwindow* window, Camera* camera) {
    this->window = window;    // Store window reference for event handling
    this->camera = camera;    // Store camera reference to manipulate it
    enabled = false;          // Control mode is disabled initially

    // Set initial camera direction from the camera's direction vector
    glm::vec3 dir = glm::normalize(camera->direction);
    pitch = glm::asin(dir.y);  // Calculate pitch based on camera's Y direction
    yaw   = glm::atan(-dir.z, dir.x);  // Calculate yaw based on camera's X and Z direction
}

void CameraController::update(float dt) {
    if (!window || !camera) return;  // Return if window or camera is invalid

    // Mouse button toggles control mode on/off
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        if (!enabled) {
            // Disable the cursor when control mode is enabled (FPS style)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            // Record the initial mouse position
            glfwGetCursorPos(window, &last_mouse_position.x, &last_mouse_position.y);

            enabled = true;  // Set control mode as enabled
        }
    } else {
        if (enabled) {
            // Re-enable the cursor when control mode is disabled
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            enabled = false;  // Disable control mode
        }
    }

    if (!enabled) return;  // If not enabled, exit the function

    // Handle mouse movement when control mode is enabled
    glm::dvec2 curr_mouse;
    glfwGetCursorPos(window, &curr_mouse.x, &curr_mouse.y);  // Get current mouse position
    glm::vec2 delta_mouse = glm::vec2(curr_mouse - last_mouse_position);  // Calculate movement delta
    last_mouse_position = curr_mouse;  // Update the last known mouse position

    // Update pitch and yaw based on mouse movement
    pitch -= delta_mouse.y * mouse_sensitivity;  // Sensitivity scales mouse movement
    yaw   -= delta_mouse.x * mouse_sensitivity;

    // Constrain pitch to avoid gimbal lock (limit to ±89.8 degrees)
    pitch = glm::clamp(pitch, -glm::half_pi<float>() * 0.98f, glm::half_pi<float>() * 0.98f);

    // Ensure yaw stays within the 0-2π range (wrap around 360 degrees)
    yaw   = glm::mod(yaw, glm::two_pi<float>());

    // Update camera's direction vector based on new pitch and yaw values
    camera->direction = glm::vec3(
        glm::cos(pitch) * glm::cos(yaw),  // X component based on pitch and yaw
        glm::sin(pitch),                  // Y component based on pitch
        glm::cos(pitch) * -glm::sin(yaw)  // Z component based on pitch and yaw
    );

    // Movement: First, calculate the right vector for strafing
    glm::vec3 right = glm::normalize(glm::cross(camera->direction, camera->up));  // Right vector (perpendicular to camera direction and up)
    glm::vec3 movement(0.0f);  // Movement vector to accumulate directional input

    // Check for key inputs to control movement (WASD, QE for up/down)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement += camera->direction;  // Move forward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement -= camera->direction;  // Move backward
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement += right;              // Move right (strafe)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement -= right;              // Move left (strafe)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement += camera->up;         // Move up
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement -= camera->up;         // Move down

    // If there is any movement input (non-zero movement vector)
    if (movement != glm::vec3(0.0f)) {
        movement = glm::normalize(movement);  // Normalize movement vector to avoid faster diagonal movement

        // Determine movement speed based on whether the shift key is pressed (for running)
        float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                        ? run_movement_speed  // Faster speed for running
                        : normal_movement_speed;  // Normal speed for walking

        // Update camera position based on movement, speed, and time delta (dt)
        camera->position += movement * speed * dt;
    }
}