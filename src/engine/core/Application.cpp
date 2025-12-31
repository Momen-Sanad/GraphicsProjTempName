#include "Application.hpp"

#include <chrono>

Application::Application(const ApplicationConfig& config)
    : m_config(config),
      m_window(config.width, config.height, config.title) {}

int Application::run(GameLayer& game) {
    if (!m_window.get_handle()) {
        return 1;
    }

    if (!m_engine.init(m_window)) {
        return 1;
    }

    game.onInit(m_engine);

    m_engine.scheduler().addSystem(FramePhase::FixedUpdate, [&game, this](float dt) {
        game.onFixedUpdate(m_engine, dt);
    });
    m_engine.scheduler().addSystem(FramePhase::Update, [&game, this](float dt) {
        game.onUpdate(m_engine, dt);
    });
    m_engine.scheduler().addSystem(FramePhase::Render, [&game, this](float dt) {
        game.onRender(m_engine, dt);
    });

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();

    while (!m_window.should_close()) {
        auto now = clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        m_window.poll_events();
        m_engine.tick(dt);
        m_window.swap_buffers();
    }

    game.onShutdown(m_engine);
    m_engine.shutdown();

    return 0;
}
