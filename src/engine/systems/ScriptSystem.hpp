#pragma once

class World;

class ScriptSystem {
public:
    explicit ScriptSystem(World& world);

    void update(float deltaSeconds);

private:
    World& m_world;
};
