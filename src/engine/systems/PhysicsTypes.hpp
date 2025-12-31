#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace gproj::physics {

using EntityId      = unsigned int;
using PhysicsBodyId = unsigned int;

// What the physics backend uses internally
struct PhysicsTransform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
};

enum class BodyType {
    Static,
    Dynamic,
    Kinematic
};

struct RigidBodyDesc {
    PhysicsTransform transform{};
    BodyType type{BodyType::Dynamic};

    glm::vec3 initialVelocity{0.0f, 0.0f, 0.0f};
    float mass{1.0f};
    bool enabled{true};

    float linearDamping{0.01f};
};

}
