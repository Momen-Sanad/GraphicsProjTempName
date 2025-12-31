#include "Enemy.hpp"

Enemy::Enemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial)
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

}

void Enemy::setPosition(const glm::vec3& p) {
    if (root) {
        root->setPosition(p);
    }
}


std::unique_ptr<Enemy> CreateEnemy(World& world, Entity* parent, MeshRenderer* bodyMesh, Material* bodyMaterial) {
    return std::make_unique<Enemy>(world, parent, bodyMesh, bodyMaterial);
}
