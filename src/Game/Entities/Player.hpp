#pragma once

#include "../GameComponents.hpp"

#include "../../engine/assets/Material.hpp"
#include "../../engine/components/CameraFollowComponent.hpp"
#include "../../engine/components/MeshRenderer.hpp"
#include "../../engine/ecs/World.hpp"

#include <glm/glm.hpp>

#include <memory>

class Player {
public:
    Player(
        World& world,
        engine::ecs::EntityId parent,
        std::shared_ptr<MeshRenderer> bodyMesh,
        std::shared_ptr<Material> bodyMaterial,
        std::shared_ptr<MeshRenderer> weaponMesh,
        std::shared_ptr<Material> weaponMaterial);

    virtual ~Player() = default;

    engine::ecs::EntityId entity() const { return root; }
    engine::ecs::EntityId getBody() const { return body; }
    engine::ecs::EntityId getWeapon() const { return weapon; }

    void setInput(const PlayerInput& input);
    virtual void update(float deltaTime);

    void setPosition(const glm::vec3& position);
    glm::vec3 getPosition() const;

    void setMoveSpeed(float speed);
    void setBlockSpeedMultiplier(float multiplier);
    void setDodgeSpeed(float speed);
    void setAttackTimings(float duration, float cooldown);
    void setDodgeTimings(float duration, float cooldown);

    void attachCamera(Camera* camera, const glm::vec3& offset, const glm::vec3& lookOffset);

    bool isBlocking() const;
    bool isAttacking() const;
    bool isDodging() const;

protected:
    game::PlayerController* controller();
    const game::PlayerController* controller() const;

    World& worldRef;
    engine::ecs::EntityId root = engine::ecs::InvalidEntity;
    engine::ecs::EntityId body = engine::ecs::InvalidEntity;
    engine::ecs::EntityId weapon = engine::ecs::InvalidEntity;

    std::unique_ptr<CameraFollowPlayer> cameraFollow;
};
