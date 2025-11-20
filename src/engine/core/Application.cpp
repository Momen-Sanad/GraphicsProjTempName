#include "Application.hpp"
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <engine/scripting/MonoBehaviour.hpp>  // For MonoBehaviour class

Application::Application(int width, int height, const std::string& title)
    : m_width(width), m_height(height), m_title(title) {}

Application::~Application() {
    Shutdown();
}

void Application::Init() {
    if (!glfwInit()) {
        Logger::Error("Failed to initialize GLFW");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    if (!m_window) {
        Logger::Error("Failed to create GLFW window");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Logger::Error("Failed to initialize OpenGL loader");
        glfwTerminate();
        exit(1);
    }

    // Initialize systems
    m_shaderManager = new ShaderManager();
    m_renderSystem = new RenderSystem();
}

void Application::Run(MonoBehaviour* behaviour) {
    // Initialize the MonoBehaviour
    behaviour->Start();

    while (!glfwWindowShouldClose(m_window)) {
        float deltaTime = 1.0f / 60.0f;  // Here you can calculate time difference between frames
        behaviour->Update(deltaTime);

        // Render the scene
        m_renderSystem->Render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }

    behaviour->OnExit();
}

void Application::Shutdown() {
    delete m_shaderManager;
    delete m_renderSystem;
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
