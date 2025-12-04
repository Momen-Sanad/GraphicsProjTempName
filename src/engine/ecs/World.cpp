#include "World.hpp"

World::World() = default;

Entity* World::createEntityWithParams(Entity* parent,
                                      const glm::vec3& position,
                                      const glm::quat& rotation,
                                      const glm::vec3& scale,
                                      MeshRenderer* mesh,
                                      Material* material)
{
    return manager.createEntityWithParams(parent, position, rotation, scale, mesh, material);
}

Entity* World::add_entity() {
    return manager.createEntity(); // default root entity
}

void World::removeEntity(Entity* entity) {
    manager.destroyEntity(entity);
}

void World::clear() {
    manager.clear(); // destroy all entities inside EntityManager
}