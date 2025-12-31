#include "Enemy.hpp"
#include "../../engine/components/MeshRenderer.hpp"

Enemy::Enemy(World& world, Entity* parent, GpuMesh* bodyMesh, Material* bodyMaterial)
    : worldRef(world) {
    root = worldRef.createEntityWithParams(parent);
    body = worldRef.createEntityWithParams(
        root,
        {0.0f, 1.0f, 0.0f},
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        {1.0f, 2.0f, 1.0f}
    );

    if (body) {
        auto& renderer = body->addComponent<MeshRenderer>();
        renderer.mesh = bodyMesh;
        renderer.material = bodyMaterial;
    }

}

void Enemy::setPosition(const glm::vec3& p) {
    if (root) {
        root->setPosition(p);
    }
}


std::unique_ptr<Enemy> CreateEnemy(World& world, Entity* parent, GpuMesh* bodyMesh, Material* bodyMaterial) {
    return std::make_unique<Enemy>(world, parent, bodyMesh, bodyMaterial);
}
