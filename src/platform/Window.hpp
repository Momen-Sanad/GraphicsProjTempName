#pragma once
#include <string>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdexcept>

// Simple window wrapper: initializes GLFW, creates window, loads GLAD,
// computes delta time and exposes a tiny API used by Application.
class Window {

private:

    GLFWwindow* handle;
    GLFWwindow* m_Window = nullptr;
    double m_lastTime = 0.0;


public:

    
    Window(int width, int height, const std::string& title);

    bool ShouldClose() const;
    void PollEvents() const;
    void SwapBuffers() const;

    // Returns delta time (seconds) since last call.
    float GetDeltaTime();

    ~Window();
    
    // Access to the raw GLFW window if needed
    GLFWwindow* GetNativeWindow() const { return m_Window; }
};
