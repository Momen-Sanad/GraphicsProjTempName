#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool should_close() const;
    void poll_events();
    void swap_buffers();

    GLFWwindow* get_handle() const { return handle; }
    void get_framebuffer_size(int& w, int& h) const;

private:
    GLFWwindow* handle = nullptr;
};

#endif