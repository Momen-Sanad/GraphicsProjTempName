#pragma once

#include "FrameScheduler.hpp"
#include "../ecs/World.hpp"
#include <memory>

class AssetManager;
class SceneManager;
class Window;

class RenderSystem;
class PhysicsSystem;
class ScriptSystem;
class TransformSystem;

namespace gproj::physics {
class PhysicsBackend;
}

class Engine {
public:
    Engine();
    ~Engine();

    bool init(Window& window);
    void tick(float deltaSeconds);
    void shutdown();

    World& world() { return m_world; }
    const World& world() const { return m_world; }

    FrameScheduler& scheduler() { return m_scheduler; }
    const FrameScheduler& scheduler() const { return m_scheduler; }

    AssetManager& assets();
    SceneManager& scenes();

    Window& window();

private:
    void registerCoreComponents();
    void registerCoreSystems();

    Window* m_window = nullptr;
    World m_world;
    FrameScheduler m_scheduler;

    std::unique_ptr<AssetManager> m_assets;
    std::unique_ptr<SceneManager> m_scenes;
    std::unique_ptr<gproj::physics::PhysicsBackend> m_physicsBackend;

    std::unique_ptr<TransformSystem> m_transformSystem;
    std::unique_ptr<PhysicsSystem> m_physicsSystem;
    std::unique_ptr<RenderSystem> m_renderSystem;
    std::unique_ptr<ScriptSystem> m_scriptSystem;
};
