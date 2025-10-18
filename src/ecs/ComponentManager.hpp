/* Stores data for each component type. Manages adding/removing components. */
#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <cassert>

namespace ecs {

using EntityId = uint32_t;
using ComponentTypeId = std::size_t;

// Simple component type ID generator
inline ComponentTypeId GetUniqueComponentTypeId() {
    static ComponentTypeId lastId = 0u;
    return lastId++;
}

// A runtime mapping from std::type_index -> ComponentTypeId
class ComponentTypeRegistry {
public:
    template<typename T>
    static ComponentTypeId GetComponentType() {
        const std::type_index ti(typeid(T));
        auto it = registry().find(ti);
        if (it != registry().end())
            return it->second;
        
        ComponentTypeId id = GetUniqueComponentTypeId();
        registry().emplace(ti, id);
        return id;
    }

private:
    static std::unordered_map<std::type_index, ComponentTypeId>& registry() {
        static std::unordered_map<std::type_index, ComponentTypeId> reg;
        return reg;
    }
};

// ComponentArray: stores components of type T indexed by EntityId
template<typename T>
class ComponentArray {
public:
    void InsertData(EntityId entity, const T& component) {
        assert(data.find(entity) == data.end() && "Component added to same entity more than once.");
        data.emplace(entity, component);
    }

    void RemoveData(EntityId entity) {
        data.erase(entity);
    }

    T& GetData(EntityId entity) {
        assert(data.find(entity) != data.end() && "Retrieving non-existent component.");
        return data.at(entity);
    }

    bool HasData(EntityId entity) const {
        return data.find(entity) != data.end();
    }

    void EntityDestroyed(EntityId entity) {
        if (data.find(entity) != data.end()) data.erase(entity);
    }

private:
    std::unordered_map<EntityId, T> data;
};

// ComponentManager: hold arrays for all registered component types (type-erased)
class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        const ComponentTypeId typeId = ComponentTypeRegistry::GetComponentType<T>();
        assert(componentArrays.find(typeId) == componentArrays.end() && "Registering component type more than once.");
        componentArrays.emplace(typeId, std::make_shared<ErasedComponentArray<T>>());
    }

    template<typename T>
    void AddComponent(EntityId entity, const T& component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(EntityId entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& GetComponent(EntityId entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    template<typename T>
    bool HasComponent(EntityId entity) {
        return GetComponentArray<T>()->HasData(entity);
    }

    void EntityDestroyed(EntityId entity) {
        for (auto const &pair : componentArrays) {
            pair.second->EntityDestroyed(entity);
        }
    }

private:
    // Type-erased wrapper around ComponentArray<T>
    struct IComponentArray {
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(EntityId entity) = 0;
    };

    template<typename T>
    struct ErasedComponentArray : IComponentArray {
        ComponentArray<T> arr;
        void EntityDestroyed(EntityId entity) override { arr.EntityDestroyed(entity); }
    };

    template<typename T>
    std::shared_ptr< ErasedComponentArray<T> > GetComponentArray() {
        const ComponentTypeId typeId = ComponentTypeRegistry::GetComponentType<T>();
        auto it = componentArrays.find(typeId);
        assert(it != componentArrays.end() && "Component not registered before use.");
        return std::static_pointer_cast<ErasedComponentArray<T>>(it->second);
    }

    std::unordered_map<ComponentTypeId, std::shared_ptr<IComponentArray>> componentArrays;
};

}