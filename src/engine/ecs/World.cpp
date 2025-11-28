#include "World.hpp"

Entity* World::add_entity() {
    Entity* entity = new Entity();
    entities.push_back(entity);
    return entity;
}

void World::remove_entity(Entity* entity) {
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it != entities.end()) {
        delete *it;
        entities.erase(it);
    }
}

void World::update(float deltaTime) {
    for (Entity* entity : entities) {
        entity->update(deltaTime);
    }
}

void World::render() {
    for (Entity* entity : entities) {
        entity->render();
    }
}