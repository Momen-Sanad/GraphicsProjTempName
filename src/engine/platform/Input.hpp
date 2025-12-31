#ifndef INPUT_HPP
#define INPUT_HPP

#include "Window.hpp"
#include <glm/glm.hpp>

namespace Input {
    inline bool isKeyDown(const Window& window, int key) {
        return glfwGetKey(window.get_handle(), key) == GLFW_PRESS;
    }

    inline bool isMouseDown(const Window& window, int button) {
        return glfwGetMouseButton(window.get_handle(), button) == GLFW_PRESS;
    }

    inline glm::vec2 mousePosition(const Window& window) {
        double x = 0.0;
        double y = 0.0;
        glfwGetCursorPos(window.get_handle(), &x, &y);
        return glm::vec2(static_cast<float>(x), static_cast<float>(y));
    }

    inline glm::vec2 mouseDelta(const Window& window) {
        static glm::vec2 last = mousePosition(window);
        glm::vec2 current = mousePosition(window);
        glm::vec2 delta = current - last;
        last = current;
        return delta;
    }
}

#endif
