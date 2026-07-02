#pragma once

#include "Registry.hpp"
#include "SystemManager.hpp"

#include <utility>

namespace engine::ecs {

class Coordinator {
public:
    EntityId createEntity() { return registry_.createEntity(); }
    void destroyEntity(EntityId id) { registry_.destroyEntity(id); }
    void clear() {
        systems_.clear();
        registry_.clear();
    }

    Registry& registry() { return registry_; }
    const Registry& registry() const { return registry_; }

    SystemManager& systems() { return systems_; }
    const SystemManager& systems() const { return systems_; }

    template <typename T, typename... Args>
    T& addComponent(EntityId id, Args&&... args) {
        return registry_.emplace<T>(id, std::forward<Args>(args)...);
    }

    template <typename T>
    T& ensureComponent(EntityId id) {
        return registry_.ensure<T>(id);
    }

    template <typename T>
    T* getComponent(EntityId id) {
        return registry_.get<T>(id);
    }

    template <typename T>
    const T* getComponent(EntityId id) const {
        return registry_.get<T>(id);
    }

    template <typename T>
    bool hasComponent(EntityId id) const {
        return registry_.has<T>(id);
    }

    template <typename T>
    void removeComponent(EntityId id) {
        registry_.remove<T>(id);
    }

private:
    Registry registry_;
    SystemManager systems_;
};

} // namespace engine::ecs
