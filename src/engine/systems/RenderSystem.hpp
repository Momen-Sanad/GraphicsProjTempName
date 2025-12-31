#pragma once

class World;
class Window;

class RenderSystem {
public:
    RenderSystem(World& world, Window& window);

    void render(float deltaSeconds);

private:
    World& m_world;
    Window& m_window;
};
