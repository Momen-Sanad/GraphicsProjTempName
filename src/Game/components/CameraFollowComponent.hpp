/* this is only viable when we've migrated to full ECS
#pragma once

#include <glm/glm.hpp>
#include "../ecs/Component.hpp"

class Entity;
class Camera;

class CameraFollowPlayer : public Component {
public:
    CameraFollowPlayer(Entity* owner);

    void setTarget(Entity* target);
    void setOffsets(const glm::vec3& positionOffset,
                    const glm::vec3& lookOffset);

    void update(float dt);

private:
    Entity* targetEntity = nullptr;
    Camera* camera = nullptr;

    glm::vec3 positionOffset{0.0f, 2.5f, 6.0f};
    glm::vec3 lookOffset{0.0f, 1.0f, 0.0f};
}; 
*/

#pragma once

#include <glm/glm.hpp>

class Entity;
class Camera;

class CameraFollowPlayer {
public:
    CameraFollowPlayer(Camera* camera,
                       Entity* target);

    void setOffsets(const glm::vec3& positionOffset,
                    const glm::vec3& lookOffset);

    void update(float dt);
    
    void setTarget(Entity* target);

private:
    Camera* camera = nullptr;
    Entity* targetEntity = nullptr;

    glm::vec3 positionOffset{0.0f, 2.5f, 6.0f};
    glm::vec3 lookOffset{0.0f, 1.0f, 0.0f};
};