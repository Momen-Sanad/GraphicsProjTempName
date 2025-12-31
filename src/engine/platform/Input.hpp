#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>

namespace Input {
    inline bool key_down(GLFWwindow* w, int key) { return glfwGetKey(w, key) == GLFW_PRESS; }
    inline bool mouse_down(GLFWwindow* w, int button) { return glfwGetMouseButton(w, button) == GLFW_PRESS; }
}

#endif