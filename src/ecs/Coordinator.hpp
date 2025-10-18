/* High-level interface that ties all managers together (used by Engine or Application). 

example usage:

Coordinator gCoordinator;

Entity player = gCoordinator.CreateEntity();
gCoordinator.AddComponent(player, Transform{...});
gCoordinator.AddComponent(player, MeshRenderer{...}); */

#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "SystemManager.hpp"
#include <bitset>
#include <typeindex>
#include <unordered_map>

namespace ecs {

using EntityId = uint32_t;
using Signature = uint64_t; // simple fixed-size signature (up to 64 component types)

class Coordinator {
public:
    void Init() {
        componentManager = std::make_unique<ComponentManager>();
        entityManager    = std::make_unique<EntityManager>();
        systemManager    = std::make_unique<SystemManager>();
    }

    // Entity interface
    EntityId CreateEntity() { return entityManager->CreateEntity(); }
    void DestroyEntity(EntityId entity) {
        entityManager->DestroyEntity(entity);
        componentManager->EntityDestroyed(entity);
        systemManager->EntityDestroyed(entity);
    }

    // Component interface
    template<typename T>
    void RegisterComponent() {
        componentManager->RegisterComponent<T>();
    }

    template<typename T>
    void AddComponent(EntityId entity, const T& component) {
        componentManager->AddComponent<T>(entity, component);
        // set bit in signature
        Signature sig = entityManager->GetSignature(entity);
        const ComponentTypeId typeId = ComponentTypeRegistry::GetComponentType<T>();
        sig |= (1ULL << typeId);
        entityManager->SetSignature(entity, sig);
        systemManager->EntitySignatureChanged(entity, sig);
    }

    template<typename T>
    void RemoveComponent(EntityId entity) {
        componentManager->RemoveComponent<T>(entity);
        Signature sig = entityManager->GetSignature(entity);
        const ComponentTypeId typeId = ComponentTypeRegistry::GetComponentType<T>();
        sig &= ~(1ULL << typeId);
        entityManager->SetSignature(entity, sig);
        systemManager->EntitySignatureChanged(entity, sig);
    }

    template<typename T>
    T& GetComponent(EntityId entity) {
        return componentManager->GetComponent<T>(entity);
    }

    template<typename T>
    bool HasComponent(EntityId entity) {
        return componentManager->HasComponent<T>(entity);
    }

    // System interface
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return systemManager->RegisterSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature) {
        systemManager->SetSignature<T>(signature);
    }

    template<typename T>
    std::shared_ptr<T> GetSystem() {
        return systemManager->GetSystem<T>();
    }

    // Utility
    std::vector<EntityId> GetAllEntities() const { return entityManager->GetAllEntities(); }

private:
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<SystemManager> systemManager;
};

}