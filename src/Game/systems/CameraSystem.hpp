// follow camera smoothing etc.
/* example:
 *
 
void CameraSystem::update(float dt) {
    for (auto &e : entitiesWithCameraFollow) {
        auto transform = e.getComponent<TransformComponent>();
        auto follow = e.getComponent<CameraFollowComponent>();
        glm::vec3 target = follow.targetEntity->getComponent<TransformComponent>().worldPosition + follow.offset;
        glm::vec3 pos = glm::mix(transform.worldPosition, target, 1.0f - powf(0.001f, dt * follow.smoothness));
        transform.setWorldPosition(pos);
    }
}

 *
 */