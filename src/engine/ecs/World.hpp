#pragma once

#include "Component.hpp"
#include "Entity.hpp"
#include "Types.hpp"
#include "../components/Camera.hpp"
#include "../components/Transform.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <utility>
#include <vector>

class World {
public:
    World();
    ~World() = default;

    Entity* createEntity();
    Entity* createEntityWithParams(Entity* parent = nullptr,
                                   const glm::vec3& position = glm::vec3(0.f),
                                   const glm::quat& rotation = glm::quat(1, 0, 0, 0),
                                   const glm::vec3& scale = glm::vec3(1.f));

    Entity* add_entity();
    void removeEntity(Entity* entity);
    void clear();

    bool isAlive(EntityId id) const;
    Entity* getEntity(EntityId id) const;
    std::vector<Entity*> getEntities() const;
    std::vector<Entity*> getRoots() const;
    std::vector<Entity*> getChildren(EntityId parentId) const;

    Camera& get_camera();
    const Camera& get_camera() const;
    void setMainCamera(Entity* entity);
    Entity* getMainCamera() const;

    ComponentRegistry& components() { return registry; }
    const ComponentRegistry& components() const { return registry; }

    template <typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args);

    template <typename T>
    bool hasComponent(EntityId id) const;

    template <typename T>
    T& getComponent(EntityId id);

    template <typename T>
    const T& getComponent(EntityId id) const;

    template <typename T>
    void removeComponent(EntityId id);

private:
    struct EntitySlot {
        bool alive = false;
        std::unique_ptr<Entity> handle;
    };

    std::vector<EntitySlot> entities;
    EntityId nextId = 0;
    EntityId mainCameraId = kInvalidEntity;
    ComponentRegistry registry;
};

template <typename T, typename... Args>
T& World::addComponent(EntityId id, Args&&... args) {
    return registry.storage<T>().emplace(id, std::forward<Args>(args)...);
}

template <typename T>
bool World::hasComponent(EntityId id) const {
    if (!registry.hasStorage<T>()) {
        return false;
    }
    return registry.storage<T>().has(id);
}

template <typename T>
T& World::getComponent(EntityId id) {
    return registry.storage<T>().get(id);
}

template <typename T>
const T& World::getComponent(EntityId id) const {
    return registry.storage<T>().get(id);
}

template <typename T>
void World::removeComponent(EntityId id) {
    if (!registry.hasStorage<T>()) {
        return;
    }
    registry.storage<T>().remove(id);
}
