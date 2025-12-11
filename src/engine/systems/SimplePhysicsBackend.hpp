#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include "engine/physics/IPhysicsBackend.hpp"

namespace gproj::physics {

struct SimpleBodyState {
    PhysicsTransform transform;
    glm::vec3 velocity{0.0f, 0.0f, 0.0f};
    BodyType type{BodyType::Dynamic};
    float mass{1.0f};
    float linearDamping{0.01f};
    bool enabled{true};
};

class SimplePhysicsBackend final : public IPhysicsBackend {
public:
    SimplePhysicsBackend();
    ~SimplePhysicsBackend() override;

    void initialize() override;
    void shutdown() override;

    PhysicsBodyId createRigidBody(const RigidBodyDesc& desc) override;
    void          destroyRigidBody(PhysicsBodyId bodyId) override;

    void setBodyTransform(PhysicsBodyId bodyId,
                          const PhysicsTransform& transform) override;
    [[nodiscard]] PhysicsTransform
    getBodyTransform(PhysicsBodyId bodyId) const override;

    void stepSimulation(float dtSeconds) override;

    void setGravity(const glm::vec3& g) { m_gravity = g; }

private:
    std::unordered_map<PhysicsBodyId, SimpleBodyState> m_bodies;
    PhysicsBodyId m_nextId{1};

    glm::vec3 m_gravity{0.0f, -9.81f, 0.0f};

    void integrateBody(SimpleBodyState& body, float dtSeconds);
};

} // namespace gproj::physics
