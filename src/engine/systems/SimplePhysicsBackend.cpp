#include "engine/physics/SimplePhysicsBackend.hpp"

namespace gproj::physics {

SimplePhysicsBackend::SimplePhysicsBackend() = default;
SimplePhysicsBackend::~SimplePhysicsBackend() = default;

void SimplePhysicsBackend::initialize() {
    // nothing yet
}

void SimplePhysicsBackend::shutdown() {
    m_bodies.clear();
}

PhysicsBodyId SimplePhysicsBackend::createRigidBody(const RigidBodyDesc& desc) {
    PhysicsBodyId id = m_nextId++;

    SimpleBodyState state;
    state.transform      = desc.transform;
    state.velocity       = desc.initialVelocity;
    state.type           = desc.type;
    state.mass           = (desc.mass <= 0.0f ? 1.0f : desc.mass);
    state.enabled        = desc.enabled;
    state.linearDamping  = desc.linearDamping;

    m_bodies.emplace(id, state);
    return id;
}

void SimplePhysicsBackend::destroyRigidBody(PhysicsBodyId bodyId) {
    m_bodies.erase(bodyId);
}

void SimplePhysicsBackend::setBodyTransform(PhysicsBodyId bodyId,
                                            const PhysicsTransform& transform) {
    auto it = m_bodies.find(bodyId);
    if (it == m_bodies.end()) return;
    it->second.transform = transform;
}

PhysicsTransform
SimplePhysicsBackend::getBodyTransform(PhysicsBodyId bodyId) const {
    auto it = m_bodies.find(bodyId);
    if (it == m_bodies.end()) {
        return PhysicsTransform{};
    }
    return it->second.transform;
}

void SimplePhysicsBackend::stepSimulation(float dtSeconds) {
    if (dtSeconds <= 0.0f) return;

    for (auto& [id, body] : m_bodies) {
        if (!body.enabled) continue;
        integrateBody(body, dtSeconds);
    }
}

void SimplePhysicsBackend::integrateBody(SimpleBodyState& body, float dtSeconds) {
    switch (body.type) {
        case BodyType::Static:
            return; // no movement
        case BodyType::Kinematic:
            // just move by velocity, ignore gravity
            body.transform.position += body.velocity * dtSeconds;
            return;
        case BodyType::Dynamic:
        default:
            break;
    }

    // Basic semi-implicit Euler
    glm::vec3 acceleration = m_gravity;

    body.velocity += acceleration * dtSeconds;
    body.velocity *= (1.0f - body.linearDamping);

    body.transform.position += body.velocity * dtSeconds;
}

}
