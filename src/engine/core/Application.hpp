#pragma once

#include "Engine.hpp"
#include "GameLayer.hpp"
#include "../platform/Window.hpp"

struct ApplicationConfig {
    int width = 1280;
    int height = 720;
    const char* title = "Game";
};

class Application {
public:
    explicit Application(const ApplicationConfig& config);

    int run(GameLayer& game);

    Engine& engine() { return m_engine; }
    Window& window() { return m_window; }

private:
    ApplicationConfig m_config;
    Window m_window;
    Engine m_engine;
};
