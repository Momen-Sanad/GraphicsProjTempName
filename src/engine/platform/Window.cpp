#include "Window.hpp"
#include <cstdio>

Window::Window(int width, int height, const char* title) {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!handle) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(handle);
}

Window::~Window() {
    if (handle) glfwDestroyWindow(handle);
    glfwTerminate();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(handle);
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::swap_buffers() {
    glfwSwapBuffers(handle);
}

void Window::get_framebuffer_size(int& w, int& h) const {
    glfwGetFramebufferSize(handle, &w, &h);
}