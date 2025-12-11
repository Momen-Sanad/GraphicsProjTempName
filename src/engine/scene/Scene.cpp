#include "Scene.hpp"
#include <algorithm>

using namespace std;

Scene::EntityData* Scene::findEntity(const string& name) {
    auto it = find_if(entities.begin(), entities.end(), 
        [&name](const EntityData& entity) { return entity.name == name; });
    return (it != entities.end()) ? &(*it) : nullptr;
}

void Scene::addEntity(const EntityData& entity) {
    entities.push_back(entity);
}

void Scene::removeEntity(const string& name) {
    entities.erase(
        remove_if(entities.begin(), entities.end(),
            [&name](const EntityData& entity) { return entity.name == name; }),
        entities.end());
}

bool Scene::hasEntity(const string& name) const {
    return find_if(entities.begin(), entities.end(),
        [&name](const EntityData& entity) { return entity.name == name; }) != entities.end();
}

size_t Scene::getEntityCount() const {
    return entities.size();
}

void Scene::clear() {
    entities.clear();
    name.clear();
}