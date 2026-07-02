#include "CameraFollowComponent.hpp"

#include "Camera.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

#include <glm/geometric.hpp>

CameraFollowPlayer::CameraFollowPlayer(
    Camera* cam,
    engine::ecs::Registry* targetRegistry,
    engine::ecs::EntityId target)
    : camera(cam)
    , registry(targetRegistry)
    , targetEntity(target)
{
}

void CameraFollowPlayer::setTarget(engine::ecs::EntityId target)
{
    targetEntity = target;
}

void CameraFollowPlayer::setOffsets(const glm::vec3& pos, const glm::vec3& look)
{
    positionOffset = pos;
    lookOffset = look;
}

void CameraFollowPlayer::update(float)
{
    if (!camera || !registry || !registry->isAlive(targetEntity)) {
        return;
    }

    const auto* transform = registry->get<engine::ecs::Transform>(targetEntity);
    if (!transform) {
        return;
    }

    const glm::vec3 targetPos = glm::vec3(transform->worldMatrix[3]);
    const glm::vec3 camPos = targetPos + positionOffset;
    const glm::vec3 lookAt = targetPos + lookOffset;

    camera->position = camPos;
    camera->direction = glm::normalize(lookAt - camPos);
    camera->up = glm::vec3(0.0f, 1.0f, 0.0f);
}
