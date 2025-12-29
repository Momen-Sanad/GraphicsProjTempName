#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../engine/ecs/World.hpp"
#include "../../engine/components/CameraFollowComponent.hpp"
#include "../../engine/assets/Material.hpp"

struct PlayerInput {
    glm::vec2 move{0.0f};
    bool block = false;
    bool attack = false;
    bool dodge = false;
};

class Player {
public:
    Player(World& world,
           Entity* parent,
           MeshRenderer* bodyMesh,
           Material* bodyMaterial,
           MeshRenderer* weaponMesh,
           Material* weaponMaterial);

    virtual ~Player() = default;

    // Core
    Entity* entity() const { return root; }
    Entity* getBody() const { return body; }
    Entity* getWeapon() const { return weapon; }

    // Input & update
    void setInput(const PlayerInput& input) { inputState = input; }
    virtual void update(float deltaTime);

    // Transform
    void setPosition(const glm::vec3& p);
    glm::vec3 getPosition() const;

    // Tunables
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setBlockSpeedMultiplier(float multiplier);
    void setDodgeSpeed(float speed) { dodgeSpeed = speed; }
    void setAttackTimings(float duration, float cooldown);
    void setDodgeTimings(float duration, float cooldown);

    // Camera
    void attachCamera(Camera* camera,
                      const glm::vec3& offset,
                      const glm::vec3& lookOffset);

    // State queries
    bool isBlocking() const { return blocking; }
    bool isAttacking() const { return attackTimer > 0.0f; }
    bool isDodging() const { return dodgeTimer > 0.0f; }

protected:
    World& worldRef;

    Entity* root = nullptr;
    Entity* body = nullptr;
    Entity* weapon = nullptr;

    PlayerInput inputState{};
    glm::vec3 facing{0.0f, 0.0f, 1.0f};

    bool blocking = false;

    float moveSpeed = 4.0f;
    float blockSpeedMultiplier = 0.4f;

    float dodgeSpeed = 10.0f;
    float dodgeDuration = 0.2f;
    float dodgeCooldown = 0.7f;
    float dodgeTimer = 0.0f;
    float dodgeCooldownTimer = 0.0f;

    float attackDuration = 0.25f;
    float attackCooldown = 0.35f;
    float attackTimer = 0.0f;
    float attackCooldownTimer = 0.0f;

    glm::quat weaponRestRotation{1, 0, 0, 0};

    // Camera behavior (NOT ECS)
    std::unique_ptr<CameraFollowPlayer> cameraFollow;

private:
    void updateMovement(float dt);
    void updateAttack(float dt);
};
