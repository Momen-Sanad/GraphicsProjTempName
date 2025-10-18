#include "core/Application.hpp"
#include <stdexcept>

Application::Application(Window& window) : m_window(&window) {}


Application::Application(int width, int height, const char* title) {

    m_window = new Window(width, height, title);

    if (!gladLoadGL(glfwGetProcAddress)) 
        throw std::runtime_error("Failed to initialize GLAD.");
    

}

void Application::AddBehaviour(const std::shared_ptr<MonoBehaviour>& behaviour) {
    if (behaviour)
        m_behaviours.push_back(behaviour);
}

void Application::Run(MonoBehaviour* behaviour) {

    /* implement this here: 
        along with its dependencies in other files
        
    engine.Init();
    while (!window->ShouldClose()) {
        float dt = CalculateDeltaTime();
        engine.Update(dt);
        window->SwapBuffers();
    }
*/

    std::shared_ptr<MonoBehaviour> mainBehaviour(behaviour);
    m_behaviours.push_back(mainBehaviour);
    mainBehaviour->Start();

    while (!m_window->ShouldClose())
    {
        
        mainBehaviour->Update(0.016f);
        m_window->SwapBuffers();
        m_window->PollEvents();
    }

    mainBehaviour->OnExit();
}
