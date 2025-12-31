/* This is only viable when we've migrated to full ECS
#include "CameraFollowPlayer.hpp"
#include "../ecs/Entity.hpp"
#include "/Camera.hpp"

CameraFollowPlayer::CameraFollowPlayer(Entity* owner)
    : Component(owner) {
    camera = owner->getComponent<Camera>();
}

void CameraFollowPlayer::setTarget(Entity* target) {
    targetEntity = target;
}

void CameraFollowPlayer::setOffsets(const glm::vec3& pos,
                                    const glm::vec3& look) {
    positionOffset = pos;
    lookOffset = look;
}

void CameraFollowPlayer::update(float) {
    if (!camera || !targetEntity) return;

    glm::vec3 targetPos = targetEntity->getWorldPosition();
    glm::vec3 camPos = targetPos + positionOffset;
    glm::vec3 lookAt = targetPos + lookOffset;

    camera->position = camPos;
    camera->direction = glm::normalize(lookAt - camPos);
    camera->up = {0.0f, 1.0f, 0.0f};
}
*/
#include "CameraFollowComponent.hpp"
#include "../../engine/ecs/Entity.hpp"
#include "../../engine/components/Camera.hpp"

CameraFollowPlayer::CameraFollowPlayer(Camera* cam, Entity* target)
    : camera(cam), targetEntity(target) {
    // nothing else, offsets keep their defaults
}

void CameraFollowPlayer::setTarget(Entity* target) {
    targetEntity = target;
}

void CameraFollowPlayer::setOffsets(const glm::vec3& pos, const glm::vec3& look) {
    positionOffset = pos;
    lookOffset = look;
}

void CameraFollowPlayer::update(float) {
    if (!camera || !targetEntity) return;

    // Use the entity's world transform to compute the camera position & direction.
    glm::mat4 wm = targetEntity->getWorldMatrix();
    glm::vec3 targetPos = glm::vec3(wm[3]); // translation column
    glm::vec3 camPos = targetPos + positionOffset;
    glm::vec3 lookAt = targetPos + lookOffset;

    camera->position = camPos;
    camera->direction = glm::normalize(lookAt - camPos);
    camera->up = glm::vec3(0.0f, 1.0f, 0.0f);
}
