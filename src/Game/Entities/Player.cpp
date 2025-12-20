//example usage
/*
#include "Player.hpp"
#include "../engine/components/HitboxComponent.hpp"
#include "../engine/components/CameraFollowComponent.hpp"
#include "../engine/components/MeshRenderer.hpp"

Player::Player(World& world): worldRef(world) {
    pEntity = world.createEntity();              // adjust to your API
    pEntity->addComponent<TransformComponent>();
    pEntity->addComponent<MovementComponent>();
    pEntity->addComponent<HealthComponent>();
    pEntity->addComponent<HitboxComponent>();

    // create body child
    body = world.createEntity();
    body->addComponent<TransformComponent>(); // local transform relative to parent
    body->addComponent<MeshRenderer>();       // provide mesh + material
    pEntity->addChild(body);                  // or world.setParent(child, parent)

    // weapon child of body
    weapon = world.createEntity();
    weapon->addComponent<TransformComponent>();
    weapon->addComponent<MeshRenderer>();
    body->addChild(weapon);

    // attach camera (component that references this entity)
    Entity* cameraEnt = world.createEntity();
    cameraEnt->addComponent<CameraComponent>();
    auto& camFollow = cameraEnt->addComponent<CameraFollowComponent>();
    camFollow.targetEntity = pEntity;
    camFollow.offset = glm::vec3(0,2.0f, -4.0f);

    // set layer/masks as needed
    auto &hb = pEntity->getComponent<HitboxComponent>();
    hb.layer = CollisionLayer::Player;
    hb.mask = static_cast<uint32_t>(CollisionLayer::Enemy) | static_cast<uint32_t>(CollisionLayer::Terrain);
}

void Player::setPosition(const glm::vec3& p) {
    pEntity->getComponent<TransformComponent>().setLocalPosition(p);
}

*/

//usage in main.cpp or level builder or smth:
/*
std::unique_ptr<Player> CreatePlayer(World& world) {
    auto player = std::make_unique<Player>(world);
    // set initial meshes, materials, stats
    player->entity()->getComponent<HealthComponent>().maxHP = 150;
    player->entity()->getComponent<HealthComponent>().hp = 150;
    player->entity()->getComponent<MovementComponent>().maxSpeed = 12.0f;
    return player;
}
*/