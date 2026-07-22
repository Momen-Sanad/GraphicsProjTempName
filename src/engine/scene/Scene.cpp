#include "Scene.hpp"
#include <algorithm>

Scene::EntityData* Scene::findEntity(const std::string& name) {
    auto it = std::find_if(entities.begin(), entities.end(), 
        [&name](const EntityData& entity) { return entity.name == name; });
    return (it != entities.end()) ? &(*it) : nullptr;
}

void Scene::addEntity(const EntityData& entity) {
    entities.push_back(entity);
}

void Scene::removeEntity(const std::string& name) {
    entities.erase(
        std::remove_if(entities.begin(), entities.end(),
            [&name](const EntityData& entity) { return entity.name == name; }),
        entities.end());
}

bool Scene::hasEntity(const std::string& name) const {
    return std::find_if(entities.begin(), entities.end(),
        [&name](const EntityData& entity) { return entity.name == name; }) != entities.end();
}

size_t Scene::getEntityCount() const {
    return entities.size();
}

void Scene::clear() {
    entities.clear();
    name.clear();
}