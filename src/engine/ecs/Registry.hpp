#pragma once

#include "EntityId.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace engine::ecs {

class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void remove(EntityId id) = 0;
    virtual bool has(EntityId id) const = 0;
    virtual size_t size() const = 0;
};

class ComponentSignature {
public:
    template <typename T>
    void set(bool present) {
        std::type_index key(typeid(T));
        if (present) {
            types_.insert(key);
        } else {
            types_.erase(key);
        }
    }

    template <typename T>
    bool has() const {
        return types_.contains(std::type_index(typeid(T)));
    }

    void clear() { types_.clear(); }
    size_t size() const { return types_.size(); }

private:
    std::unordered_set<std::type_index> types_;
};

template <typename T>
class ComponentPool final : public IComponentPool {
public:
    template <typename... Args>
    T& emplace(EntityId id, Args&&... args) {
        auto it = sparse_.find(id.index);
        if (it != sparse_.end()) {
            size_t denseIndex = it->second;
            if (entities_[denseIndex] == id) {
                components_[denseIndex] = T{std::forward<Args>(args)...};
                return components_[denseIndex];
            }
        }

        size_t denseIndex = components_.size();
        sparse_[id.index] = denseIndex;
        entities_.push_back(id);
        components_.push_back(T{std::forward<Args>(args)...});
        return components_.back();
    }

    T& ensure(EntityId id) {
        if (T* existing = get(id)) {
            return *existing;
        }
        return emplace(id);
    }

    T* get(EntityId id) {
        auto it = sparse_.find(id.index);
        if (it == sparse_.end()) {
            return nullptr;
        }
        size_t denseIndex = it->second;
        if (denseIndex >= entities_.size() || entities_[denseIndex] != id) {
            return nullptr;
        }
        return &components_[denseIndex];
    }

    const T* get(EntityId id) const {
        auto it = sparse_.find(id.index);
        if (it == sparse_.end()) {
            return nullptr;
        }
        size_t denseIndex = it->second;
        if (denseIndex >= entities_.size() || entities_[denseIndex] != id) {
            return nullptr;
        }
        return &components_[denseIndex];
    }

    bool has(EntityId id) const override {
        return get(id) != nullptr;
    }

    void remove(EntityId id) override {
        auto it = sparse_.find(id.index);
        if (it == sparse_.end()) {
            return;
        }

        size_t denseIndex = it->second;
        if (denseIndex >= entities_.size() || entities_[denseIndex] != id) {
            sparse_.erase(it);
            return;
        }

        size_t lastIndex = components_.size() - 1;
        if (denseIndex != lastIndex) {
            components_[denseIndex] = std::move(components_[lastIndex]);
            entities_[denseIndex] = entities_[lastIndex];
            sparse_[entities_[denseIndex].index] = denseIndex;
        }

        components_.pop_back();
        entities_.pop_back();
        sparse_.erase(it);
    }

    size_t size() const override {
        return components_.size();
    }

    const std::vector<EntityId>& entities() const {
        return entities_;
    }

private:
    std::vector<T> components_;
    std::vector<EntityId> entities_;
    std::unordered_map<uint32_t, size_t> sparse_;
};

class Registry {
public:
    EntityId createEntity();
    bool isAlive(EntityId id) const;
    void destroyEntity(EntityId id);
    void clear();
    size_t aliveCount() const { return aliveCount_; }
    const ComponentSignature* signature(EntityId id) const;

    template <typename T, typename... Args>
    T& emplace(EntityId id, Args&&... args) {
        validateAlive(id);
        T& component = pool<T>().emplace(id, std::forward<Args>(args)...);
        signatures_[id.index].set<T>(true);
        return component;
    }

    template <typename T>
    T& ensure(EntityId id) {
        validateAlive(id);
        T& component = pool<T>().ensure(id);
        signatures_[id.index].set<T>(true);
        return component;
    }

    template <typename T>
    T* get(EntityId id) {
        if (!isAlive(id)) {
            return nullptr;
        }
        auto* existingPool = findPool<T>();
        return existingPool ? existingPool->get(id) : nullptr;
    }

    template <typename T>
    const T* get(EntityId id) const {
        if (!isAlive(id)) {
            return nullptr;
        }
        const auto* existingPool = findPool<T>();
        return existingPool ? existingPool->get(id) : nullptr;
    }

    template <typename T>
    bool has(EntityId id) const {
        return get<T>(id) != nullptr;
    }

    template <typename T>
    void remove(EntityId id) {
        auto* existingPool = findPool<T>();
        if (existingPool) {
            existingPool->remove(id);
        }
        if (isAlive(id)) {
            signatures_[id.index].set<T>(false);
        }
    }

    template <typename... Components>
    bool matches(EntityId id) const {
        const ComponentSignature* entitySignature = signature(id);
        return entitySignature && (entitySignature->has<Components>() && ...);
    }

    template <typename... Components, typename Fn>
    void each(Fn&& fn) {
        static_assert(sizeof...(Components) > 0, "Registry::each requires at least one component type");
        using First = typename FirstType<Components...>::type;
        auto* firstPool = findPool<First>();
        if (!firstPool) {
            return;
        }

        std::vector<EntityId> entities = firstPool->entities();
        for (EntityId id : entities) {
            if (!matches<Components...>(id)) {
                continue;
            }
            fn(id, *get<Components>(id)...);
        }
    }

    template <typename T>
    const std::vector<EntityId>& entitiesWith() const {
        static const std::vector<EntityId> empty;
        const auto* existingPool = findPool<T>();
        return existingPool ? existingPool->entities() : empty;
    }

private:
    template <typename T, typename... Rest>
    struct FirstType {
        using type = T;
    };

    template <typename T>
    ComponentPool<T>& pool() {
        std::type_index key(typeid(T));
        auto it = pools_.find(key);
        if (it == pools_.end()) {
            auto inserted = pools_.emplace(key, std::make_unique<ComponentPool<T>>());
            it = inserted.first;
        }
        return *static_cast<ComponentPool<T>*>(it->second.get());
    }

    template <typename T>
    ComponentPool<T>* findPool() {
        std::type_index key(typeid(T));
        auto it = pools_.find(key);
        return it == pools_.end() ? nullptr : static_cast<ComponentPool<T>*>(it->second.get());
    }

    template <typename T>
    const ComponentPool<T>* findPool() const {
        std::type_index key(typeid(T));
        auto it = pools_.find(key);
        return it == pools_.end() ? nullptr : static_cast<const ComponentPool<T>*>(it->second.get());
    }

    void validateAlive(EntityId id) const;

    std::vector<uint32_t> generations_;
    std::vector<bool> alive_;
    std::vector<uint32_t> freeList_;
    std::vector<ComponentSignature> signatures_;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> pools_;
    size_t aliveCount_ = 0;
};

} // namespace engine::ecs
