#include "Player.hpp"

#include <algorithm>
#include <cmath>

#include "../../engine/components/MeshRenderer.hpp"
#include "../../engine/components/Camera.hpp"
#include "../../engine/ecs/Entity.hpp"

// Helper: get world position from an entity's world matrix.
// (glm uses column-major; translation is in the 4th column)
static glm::vec3 getEntityWorldPosition(const Entity* e) {
    if (!e) return glm::vec3(0.0f);
    glm::mat4 wm = e->getWorldMatrix();
    return glm::vec3(wm[3]);
}

// Constructor: create root, body and weapon entities using World API
Player::Player(World& world,
               Entity* parent,
               MeshRenderer* bodyMesh,
               Material* bodyMaterial,
               MeshRenderer* weaponMesh,
               Material* weaponMaterial)
    : worldRef(world) {

    // create root as a simple (empty) entity and set parent if provided
    root = worldRef.add_entity();
    if (parent) {
        root->setParent(parent);
    }

    // create body as a child of root, with a local offset and the provided mesh/material
    body = worldRef.createEntityWithParams(
        root,
        /* position */ glm::vec3{0.0f, 1.0f, 0.0f},
        /* rotation */ glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
        /* scale    */ glm::vec3{1.0f, 2.0f, 1.0f},
        /* mesh     */ bodyMesh,
        /* material */ bodyMaterial
    );

    // create weapon as a child of body, with its own transform and mesh/material
    weapon = worldRef.createEntityWithParams(
        body,
        /* position */ glm::vec3{0.75f, 0.75f, 0.0f},
        /* rotation */ glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
        /* scale    */ glm::vec3{0.2f, 0.8f, 0.2f},
        /* mesh     */ weaponMesh,
        /* material */ weaponMaterial
    );

    // remember weapon rest rotation (local)
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
    if (!root) return glm::vec3(0.0f);
    // Use world position (safer if root ever has a parent)
    return getEntityWorldPosition(root);
}

void Player::setBlockSpeedMultiplier(float multiplier) {
    blockSpeedMultiplier = std::clamp(multiplier, 0.0f, 1.0f);
}

void Player::setAttackTimings(float duration, float cooldown) {
    attackDuration = std::max(0.01f, duration);
    attackCooldown = std::max(0.0f, cooldown);
}

void Player::setDodgeTimings(float duration, float cooldown) {
    dodgeDuration = std::max(0.01f, duration);
    dodgeCooldown = std::max(0.0f, cooldown);
}

void Player::attachCamera(Camera* camera,
                          const glm::vec3& offset,
                          const glm::vec3& lookOffset) {

    // create the non-ECS camera follow behavior (no Entity changes)
    cameraFollow = std::make_unique<CameraFollowPlayer>(camera, root);
    cameraFollow->setOffsets(offset, lookOffset);
}

void Player::update(float dt) {
    if (!root) return;

    blocking = inputState.block;

    attackTimer = std::max(0.0f, attackTimer - dt);
    dodgeTimer = std::max(0.0f, dodgeTimer - dt);
    attackCooldownTimer = std::max(0.0f, attackCooldownTimer - dt);
    dodgeCooldownTimer = std::max(0.0f, dodgeCooldownTimer - dt);

    if (inputState.attack && attackCooldownTimer <= 0.0f && attackTimer <= 0.0f) {
        attackTimer = attackDuration;
        attackCooldownTimer = attackCooldown;
    }

    if (inputState.dodge && dodgeCooldownTimer <= 0.0f && dodgeTimer <= 0.0f) {
        dodgeTimer = dodgeDuration;
        dodgeCooldownTimer = dodgeCooldown;
    }

    updateMovement(dt);
    updateAttack(dt);

    if (cameraFollow) {
        cameraFollow->update(dt); // use dt
    }
}

void Player::updateMovement(float dt) {
    glm::vec3 move(inputState.move.x, 0.0f, inputState.move.y);
    float len = glm::length(move);

    if (len > 0.001f) {
        move /= len;
        facing = move;
    }

    float speed = blocking ? moveSpeed * blockSpeedMultiplier : moveSpeed;

    if (dodgeTimer > 0.0f) {
        // translate local position by dodge amount
        glm::vec3 localPos = root->getPosition();
        root->setPosition(localPos + facing * dodgeSpeed * dt);
    } else if (len > 0.001f) {
        glm::vec3 localPos = root->getPosition();
        root->setPosition(localPos + move * speed * dt);
    }

    if (len > 0.001f) {
        float yaw = std::atan2(facing.x, facing.z);
        root->setRotation(glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
}

void Player::updateAttack(float) {
    if (!weapon) return;

    if (attackTimer > 0.0f) {
        float t = 1.0f - (attackTimer / attackDuration);
        float swing = std::sin(t * 3.1415926f);
        weapon->setRotation(
            weaponRestRotation *
            glm::angleAxis(-swing * 1.2f, glm::vec3(0.0f, 1.0f, 0.0f))
        );
    } else {
        weapon->setRotation(weaponRestRotation);
    }
}
