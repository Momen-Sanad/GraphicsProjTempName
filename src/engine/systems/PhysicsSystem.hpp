#pragma once

#include "PhysicsBackend.hpp"

class World;

namespace gproj::physics {

class PhysicsSystem {
public:
    PhysicsSystem(World& world, PhysicsBackend& backend);

    void initialize();
    void shutdown();

    void fixedUpdate(float dtSeconds);

private:
    World& m_world;
    PhysicsBackend& m_backend;
};

} // namespace gproj::physics
