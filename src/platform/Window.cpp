#include "platform/Window.hpp"
#include <iostream>
#include <stdexcept>

Window::Window(int width, int height, const std::string& title) {

    if (!glfwInit()) 
        throw std::runtime_error("GLFW initialization failed");
    

    // Request an OpenGL 3.3 Core context (adjust to your needs)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_Window) 
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1); // vsync on

    // Load GL function pointers using GLAD (glfwGetProcAddress provided by GLFW)
    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    m_lastTime = glfwGetTime();
}

Window::~Window() {
    if (m_Window) 
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();
}

bool Window::ShouldClose() const { return m_Window ? glfwWindowShouldClose(m_Window) : true; }

void Window::PollEvents() const { glfwPollEvents(); } 


void Window::SwapBuffers() const { 
    if (m_Window) 
    glfwSwapBuffers(m_Window); 
}


float Window::GetDeltaTime()
{
    
    double now = glfwGetTime();
    double dt = now - m_lastTime;
    // clamp small/negative dt to zero safety
    if (dt < 0.0) dt = 0.0;
    m_lastTime = now;
    return static_cast<float>(dt);
}
