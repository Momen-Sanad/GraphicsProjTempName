#pragma once

#include "PhysicsTypes.hpp"

namespace gproj::physics {

class PhysicsBackend {
public:
    virtual ~PhysicsBackend() = default;

    virtual void initialize() = 0;
    virtual void shutdown()   = 0;

    virtual PhysicsBodyId createRigidBody(const RigidBodyDesc& desc) = 0;
    virtual void          destroyRigidBody(PhysicsBodyId bodyId)     = 0;

    virtual void setBodyTransform(PhysicsBodyId bodyId,
                                  const PhysicsTransform& transform) = 0;
    [[nodiscard]] virtual PhysicsTransform
    getBodyTransform(PhysicsBodyId bodyId) const = 0;

    virtual void stepSimulation(float dtSeconds) = 0;
};

}