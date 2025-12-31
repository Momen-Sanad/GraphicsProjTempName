#include "World.hpp"
World::World() {
    Entity* cameraEntity = createEntity();
    mainCameraId = cameraEntity->id();
    addComponent<Camera>(mainCameraId);
}

Entity* World::createEntity() {
    EntityId id = nextId++;
    if (id >= entities.size()) {
        entities.resize(id + 1);
    }

    auto& slot = entities[id];
    slot.alive = true;
    slot.handle = std::make_unique<Entity>(id, this);

    addComponent<Transform>(id);
    return slot.handle.get();
}

Entity* World::createEntityWithParams(Entity* parent,
                                      const glm::vec3& position,
                                      const glm::quat& rotation,
                                      const glm::vec3& scale) {
    Entity* entity = createEntity();
    auto& tr = getComponent<Transform>(entity->id());
    tr.position = position;
    tr.rotation = rotation;
    tr.scale = scale;
    tr.parent = parent ? parent->id() : kInvalidEntity;

    return entity;
}

Entity* World::add_entity() {
    return createEntity();
}

void World::removeEntity(Entity* entity) {
    if (!entity) {
        return;
    }

    EntityId id = entity->id();
    if (!isAlive(id)) {
        return;
    }

    if (registry.hasStorage<Transform>()) {
        auto& storage = registry.storage<Transform>();
        for (auto& [childId, transform] : storage.all()) {
            if (transform.parent == id) {
                transform.parent = kInvalidEntity;
            }
        }
    }

    registry.removeAll(id);
    entities[id].alive = false;
}

void World::clear() {
    for (auto& slot : entities) {
        slot.alive = false;
    }
    registry.clearComponents();

    mainCameraId = kInvalidEntity;
    Entity* cameraEntity = createEntity();
    mainCameraId = cameraEntity->id();
    addComponent<Camera>(mainCameraId);
}

bool World::isAlive(EntityId id) const {
    return id < entities.size() && entities[id].alive;
}

Entity* World::getEntity(EntityId id) const {
    if (!isAlive(id)) {
        return nullptr;
    }
    return entities[id].handle.get();
}

std::vector<Entity*> World::getEntities() const {
    std::vector<Entity*> result;
    result.reserve(entities.size());
    for (const auto& slot : entities) {
        if (slot.alive && slot.handle) {
            result.push_back(slot.handle.get());
        }
    }
    return result;
}

std::vector<Entity*> World::getRoots() const {
    std::vector<Entity*> roots;
    if (!registry.hasStorage<Transform>()) {
        return roots;
    }

    const auto& storage = registry.storage<Transform>();
    for (const auto& [id, transform] : storage.all()) {
        if (!isAlive(id)) {
            continue;
        }
        if (transform.parent == kInvalidEntity) {
            roots.push_back(getEntity(id));
        }
    }

    return roots;
}

std::vector<Entity*> World::getChildren(EntityId parentId) const {
    std::vector<Entity*> children;
    if (!registry.hasStorage<Transform>()) {
        return children;
    }

    const auto& storage = registry.storage<Transform>();
    for (const auto& [id, transform] : storage.all()) {
        if (transform.parent == parentId && isAlive(id)) {
            children.push_back(getEntity(id));
        }
    }

    return children;
}

Camera& World::get_camera() {
    return getComponent<Camera>(mainCameraId);
}

const Camera& World::get_camera() const {
    return getComponent<Camera>(mainCameraId);
}

void World::setMainCamera(Entity* entity) {
    if (!entity || !isAlive(entity->id())) {
        return;
    }
    mainCameraId = entity->id();
    if (!hasComponent<Camera>(mainCameraId)) {
        addComponent<Camera>(mainCameraId);
    }
}

Entity* World::getMainCamera() const {
    return getEntity(mainCameraId);
}
