#pragma once

#include "../ecs/EntityId.hpp"

#include <glm/glm.hpp>

class Camera;

namespace engine::ecs {
class Registry;
}

class CameraFollowPlayer {
public:
    CameraFollowPlayer(
        Camera* camera,
        engine::ecs::Registry* registry,
        engine::ecs::EntityId target);

    void setOffsets(const glm::vec3& positionOffset, const glm::vec3& lookOffset);
    void setTarget(engine::ecs::EntityId target);
    void update(float dt);

private:
    Camera* camera = nullptr;
    engine::ecs::Registry* registry = nullptr;
    engine::ecs::EntityId targetEntity = engine::ecs::InvalidEntity;

    glm::vec3 positionOffset{0.0f, 2.5f, 6.0f};
    glm::vec3 lookOffset{0.0f, 1.0f, 0.0f};
};
