#include "Player.hpp"

#include "../GameSystems.hpp"

#include "../../engine/ecs/EcsComponents.hpp"
#include "../../engine/ecs/Registry.hpp"
#include "../../engine/systems/TransformSystem.hpp"

#include <algorithm>

Player::Player(
    World& world,
    engine::ecs::EntityId parent,
    std::shared_ptr<MeshRenderer> bodyMesh,
    std::shared_ptr<Material> bodyMaterial,
    std::shared_ptr<MeshRenderer> weaponMesh,
    std::shared_ptr<Material> weaponMaterial)
    : worldRef(world)
{
    root = worldRef.createEntity("Player", glm::vec3(0.0f));
    worldRef.setParent(root, parent);
    worldRef.registry().emplace<game::PlayerTag>(root);

    body = worldRef.createRenderable(
        "PlayerBody",
        std::move(bodyMesh),
        std::move(bodyMaterial),
        root,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 2.0f, 1.0f));

    weapon = worldRef.createRenderable(
        "PlayerWeapon",
        std::move(weaponMesh),
        std::move(weaponMaterial),
        body,
        glm::vec3(0.75f, 0.75f, 0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(0.2f, 0.8f, 0.2f));

    auto& data = worldRef.registry().emplace<game::PlayerController>(root);
    data.body = body;
    data.weapon = weapon;
    if (const auto* weaponTransform = worldRef.registry().get<engine::ecs::Transform>(weapon)) {
        data.weaponRestRotation = weaponTransform->rotation;
    }
}

void Player::setInput(const PlayerInput& input)
{
    if (auto* data = controller()) {
        data->input = input;
    }
}

void Player::update(float deltaTime)
{
    game::updatePlayerController(worldRef, root, deltaTime);
    TransformSystem::updateWorldTransforms(worldRef.registry());
    if (cameraFollow) {
        cameraFollow->update(deltaTime);
    }
}

void Player::setPosition(const glm::vec3& position)
{
    game::setLocalPosition(worldRef, root, position);
}

glm::vec3 Player::getPosition() const
{
    return game::worldPosition(const_cast<World&>(worldRef), root);
}

void Player::setMoveSpeed(float speed)
{
    if (auto* data = controller()) {
        data->moveSpeed = std::max(0.0f, speed);
    }
}

void Player::setBlockSpeedMultiplier(float multiplier)
{
    if (auto* data = controller()) {
        data->blockSpeedMultiplier = std::clamp(multiplier, 0.0f, 1.0f);
    }
}

void Player::setDodgeSpeed(float speed)
{
    if (auto* data = controller()) {
        data->dodgeSpeed = std::max(0.0f, speed);
    }
}

void Player::setAttackTimings(float duration, float cooldown)
{
    if (auto* data = controller()) {
        data->attackDuration = std::max(0.01f, duration);
        data->attackCooldown = std::max(0.0f, cooldown);
    }
}

void Player::setDodgeTimings(float duration, float cooldown)
{
    if (auto* data = controller()) {
        data->dodgeDuration = std::max(0.01f, duration);
        data->dodgeCooldown = std::max(0.0f, cooldown);
    }
}

void Player::attachCamera(Camera* camera, const glm::vec3& offset, const glm::vec3& lookOffset)
{
    cameraFollow = std::make_unique<CameraFollowPlayer>(camera, &worldRef.registry(), root);
    cameraFollow->setOffsets(offset, lookOffset);
}

bool Player::isBlocking() const
{
    const auto* data = controller();
    return data ? data->blocking : false;
}

bool Player::isAttacking() const
{
    const auto* data = controller();
    return data ? data->attackTimer > 0.0f : false;
}

bool Player::isDodging() const
{
    const auto* data = controller();
    return data ? data->dodgeTimer > 0.0f : false;
}

game::PlayerController* Player::controller()
{
    return worldRef.registry().get<game::PlayerController>(root);
}

const game::PlayerController* Player::controller() const
{
    return worldRef.registry().get<game::PlayerController>(root);
}
