#include "Player.hpp"

#include <algorithm>
#include <cmath>

Player::Player(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial, MeshRenderer* weaponMesh, Material* weaponMaterial)
    : worldRef(world) {
    root = worldRef.createEntityWithParams(parent);

    body = worldRef.createEntityWithParams(
        root,
        {0.0f, 1.0f, 0.0f},
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        {1.0f, 2.0f, 1.0f},
        bodyMesh,
        bodyMaterial
    );

    weapon = worldRef.createEntityWithParams(
        body,
        {0.75f, 0.75f, 0.0f},
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        {0.2f, 0.8f, 0.2f},
        weaponMesh,
        weaponMaterial
    );

    if (weapon) {
        weaponRestRotation = weapon->getRotation();
    }
}

void Player::setPosition(const glm::vec3& p) {
    if (root) {
        root->setPosition(p);
    }
}

glm::vec3 Player::getPosition() const {
    if (!root) {
        return glm::vec3(0.0f);
    }
    return root->getPosition();
}

void Player::setAttackTimings(float duration, float cooldown) {
    attackDuration = std::max(0.01f, duration);
    attackCooldown = std::max(0.0f, cooldown);
}

void Player::setDodgeTimings(float duration, float cooldown) {
    dodgeDuration = std::max(0.01f, duration);
    dodgeCooldown = std::max(0.0f, cooldown);
}

void Player::setCamera(Camera* cam, const glm::vec3& offset, const glm::vec3& lookOffset) {
    camera = cam;
    cameraOffset = offset;
    cameraLookOffset = lookOffset;
    updateCamera();
}

void Player::update(float deltaTime) {
    if (!root) {
        return;
    }

    blocking = inputState.block;

    attackTimer = std::max(0.0f, attackTimer - deltaTime);
    dodgeTimer = std::max(0.0f, dodgeTimer - deltaTime);
    attackCooldownTimer = std::max(0.0f, attackCooldownTimer - deltaTime);
    dodgeCooldownTimer = std::max(0.0f, dodgeCooldownTimer - deltaTime);

    if (inputState.attack && attackCooldownTimer <= 0.0f && attackTimer <= 0.0f) {
        attackTimer = attackDuration;
        attackCooldownTimer = attackCooldown;
    }

    if (inputState.dodge && dodgeCooldownTimer <= 0.0f && dodgeTimer <= 0.0f) {
        dodgeTimer = dodgeDuration;
        dodgeCooldownTimer = dodgeCooldown;
    }

    updateMovement(deltaTime);
    updateAttack();
    updateCamera();
}

void Player::updateMovement(float deltaTime) {
    glm::vec3 moveDir(inputState.move.x, 0.0f, inputState.move.y);
    float moveLen = glm::length(moveDir);

    if (moveLen > 0.001f) {
        moveDir /= moveLen;
        facing = moveDir;
    }

    float speed = moveSpeed;
    if (blocking) {
        speed *= blockSpeedMultiplier;
    }

    if (dodgeTimer > 0.0f) {
        root->setPosition(root->getPosition() + facing * dodgeSpeed * deltaTime);
    } else if (moveLen > 0.001f) {
        root->setPosition(root->getPosition() + moveDir * speed * deltaTime);
    }

    if (moveLen > 0.001f) {
        float yaw = std::atan2(facing.x, facing.z);
        root->setRotation(glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
}

void Player::updateAttack() {
    if (!weapon) {
        return;
    }

    if (attackTimer > 0.0f) {
        float t = 1.0f - (attackTimer / attackDuration);
        float swing = std::sin(t * 3.1415926f);
        weapon->setRotation(weaponRestRotation * glm::angleAxis(-swing * 1.2f, glm::vec3(0.0f, 1.0f, 0.0f)));
    } else {
        weapon->setRotation(weaponRestRotation);
    }
}

void Player::updateCamera() {
    if (!camera) {
        return;
    }

    glm::vec3 target = root->getPosition() + cameraLookOffset;
    camera->position = root->getPosition() + cameraOffset;
    camera->direction = glm::normalize(target - camera->position);
    camera->up = glm::vec3(0.0f, 1.0f, 0.0f);
}

std::unique_ptr<Player> CreatePlayer(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial, MeshRenderer* weaponMesh, Material* weaponMaterial) {
    return std::make_unique<Player>(world, parent, bodyMesh, bodyMaterial, weaponMesh, weaponMaterial);
}

std::unique_ptr<Player> CreateCrusader(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial, MeshRenderer* weaponMesh, Material* weaponMaterial) {
    auto player = CreatePlayer(world, parent, bodyMesh, bodyMaterial, weaponMesh, weaponMaterial);
    player->setMoveSpeed(3.5f);
    player->setAttackTimings(0.3f, 0.5f);
    player->setDodgeTimings(0.2f, 0.9f);
    player->setBlockSpeedMultiplier(0.3f);
    return player;
}
