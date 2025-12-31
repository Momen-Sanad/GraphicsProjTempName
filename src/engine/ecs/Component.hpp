#pragma once

#include "Types.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

struct ComponentMeta {
    std::string name;
};

class ComponentStorageBase {
public:
    virtual ~ComponentStorageBase() = default;
    virtual void remove(EntityId id) = 0;
    virtual bool has(EntityId id) const = 0;
    virtual void clear() = 0;
};

template <typename T>
class ComponentStorage final : public ComponentStorageBase {
public:
    template <typename... Args>
    T& emplace(EntityId id, Args&&... args) {
        auto [it, inserted] = components.try_emplace(id, T{std::forward<Args>(args)...});
        if (!inserted) {
            it->second = T{std::forward<Args>(args)...};
        }
        return it->second;
    }

    T& get(EntityId id) {
        return components.at(id);
    }

    const T& get(EntityId id) const {
        return components.at(id);
    }

    bool has(EntityId id) const override {
        return components.find(id) != components.end();
    }

    void remove(EntityId id) override {
        components.erase(id);
    }

    void clear() override {
        components.clear();
    }

    std::unordered_map<EntityId, T>& all() {
        return components;
    }

    const std::unordered_map<EntityId, T>& all() const {
        return components;
    }

private:
    std::unordered_map<EntityId, T> components;
};

class ComponentRegistry {
public:
    template <typename T>
    void registerComponent(const std::string& name) {
        metas[std::type_index(typeid(T))] = ComponentMeta{name};
        storage<T>();
    }

    template <typename T>
    ComponentStorage<T>& storage() {
        const auto type = std::type_index(typeid(T));
        auto& entry = storages[type];
        if (!entry) {
            entry = std::make_unique<ComponentStorage<T>>();
        }
        return *static_cast<ComponentStorage<T>*>(entry.get());
    }

    template <typename T>
    const ComponentStorage<T>& storage() const {
        const auto type = std::type_index(typeid(T));
        return *static_cast<const ComponentStorage<T>*>(storages.at(type).get());
    }

    template <typename T>
    bool isRegistered() const {
        return metas.find(std::type_index(typeid(T))) != metas.end();
    }

    template <typename T>
    bool hasStorage() const {
        return storages.find(std::type_index(typeid(T))) != storages.end();
    }

    const ComponentMeta* meta(std::type_index type) const {
        auto it = metas.find(type);
        return it == metas.end() ? nullptr : &it->second;
    }

    void removeAll(EntityId id) {
        for (auto& [_, storage] : storages) {
            storage->remove(id);
        }
    }

    void clearComponents() {
        for (auto& [_, storage] : storages) {
            storage->clear();
        }
    }

    void clear() {
        storages.clear();
        metas.clear();
    }

private:
    std::unordered_map<std::type_index, ComponentMeta> metas;
    std::unordered_map<std::type_index, std::unique_ptr<ComponentStorageBase>> storages;
};
