#include "Engine.hpp"

#include "../assets/AssetManager.hpp"
#include "../components/Camera.hpp"
#include "../components/Collider.hpp"
#include "../components/Light.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/Script.hpp"
#include "../components/Transform.hpp"
#include "../gl/GLContext.hpp"
#include "../platform/Window.hpp"
#include "../scene/SceneManager.hpp"
#include "../systems/PhysicsSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/ScriptSystem.hpp"
#include "../systems/SimplePhysicsBackend.hpp"
#include "../systems/TransformSystem.hpp"

Engine::Engine() = default;

Engine::~Engine() {
    shutdown();
}

bool Engine::init(Window& window) {
    m_window = &window;

    if (!m_window->get_handle()) {
        return false;
    }

    if (!GLContext::init()) {
        return false;
    }
    GLContext::enable_default_render_settings();

    m_assets = std::make_unique<AssetManager>();
    m_scenes = std::make_unique<SceneManager>(&m_world, m_assets.get());

    m_physicsBackend = std::make_unique<gproj::physics::SimplePhysicsBackend>();

    registerCoreComponents();
    registerCoreSystems();

    return true;
}

void Engine::tick(float deltaSeconds) {
    m_scheduler.tick(deltaSeconds);
}

void Engine::shutdown() {
    if (m_physicsSystem) {
        m_physicsSystem->shutdown();
    }

    m_scheduler.clear();

    m_transformSystem.reset();
    m_renderSystem.reset();
    m_scriptSystem.reset();
    m_physicsSystem.reset();

    m_physicsBackend.reset();
    m_scenes.reset();
    m_assets.reset();
    m_window = nullptr;
}

AssetManager& Engine::assets() {
    return *m_assets;
}

SceneManager& Engine::scenes() {
    return *m_scenes;
}

Window& Engine::window() {
    return *m_window;
}

void Engine::registerCoreComponents() {
    auto& registry = m_world.components();
    registry.registerComponent<Transform>("Transform");
    registry.registerComponent<Camera>("Camera");
    registry.registerComponent<MeshRenderer>("MeshRenderer");
    registry.registerComponent<Light>("Light");
    registry.registerComponent<Collider>("Collider");
    registry.registerComponent<Script>("Script");
}

void Engine::registerCoreSystems() {
    m_transformSystem = std::make_unique<TransformSystem>(m_world);
    m_renderSystem = std::make_unique<RenderSystem>(m_world, *m_window);
    m_scriptSystem = std::make_unique<ScriptSystem>(m_world);

    m_physicsSystem = std::make_unique<gproj::physics::PhysicsSystem>(m_world, *m_physicsBackend);
    m_physicsSystem->initialize();

    m_scheduler.addSystem(FramePhase::Update, [this](float dt) {
        m_transformSystem->update(dt);
    });

    m_scheduler.addSystem(FramePhase::FixedUpdate, [this](float dt) {
        m_physicsSystem->fixedUpdate(dt);
    });

    m_scheduler.addSystem(FramePhase::Update, [this](float dt) {
        m_scriptSystem->update(dt);
    });

    m_scheduler.addSystem(FramePhase::Render, [this](float dt) {
        m_renderSystem->render(dt);
    });
}
