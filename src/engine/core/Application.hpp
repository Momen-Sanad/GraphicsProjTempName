#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <GLFW/glfw3.h>
#include <../ecs/ShaderManager.hpp>  // Include ShaderManager
#include <../systems/RenderSystem.hpp>  // Include RenderSystem
#include <../utils/Logger.hpp>  // For logging

class Application {
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    void Init();
    void Run(MonoBehaviour* behaviour);
    void Shutdown();

private:
    int m_width, m_height;
    std::string m_title;
    GLFWwindow* m_window = nullptr;
    ShaderManager* m_shaderManager = nullptr;
    RenderSystem* m_renderSystem = nullptr;
};

#endif // APPLICATION_HPP
