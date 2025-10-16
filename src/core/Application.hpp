#pragma once
#include <memory>
#include <vector>
#include <stdexcept>
#include <glad/gl.h>

#include "platform/Window.hpp"
#include "core/MonoBehaviour.hpp"

class Application {

private:
    Window* m_window = nullptr;
    std::vector<std::shared_ptr<MonoBehaviour>> m_behaviours;
    
public:

    // Constructors
    explicit Application(int width, int height, const char* title);
    explicit Application(Window& window);

    // Behaviour management
    void AddBehaviour(const std::shared_ptr<MonoBehaviour>& behaviour);

    // Run loop with user behaviour
    void Run(MonoBehaviour* behaviour);

    ~Application() = default;

};
