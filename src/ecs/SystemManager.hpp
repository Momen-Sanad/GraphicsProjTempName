/* Holds all systems and their entity lists. */
#pragma once

#include <memory>
#include <unordered_map>
#include <set>
#include <vector>
#include <cassert>

namespace ecs {

using EntityId = uint32_t;

// Base system type: users derive from this and systems keep a set of matched entities
class System {
public:
    virtual ~System() = default;
    std::set<EntityId> entities;
};

class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        const std::type_index ti(typeid(T));
        assert(systems.find(ti) == systems.end() && "Registering system more than once.");
        auto sys = std::make_shared<T>();
        systems.emplace(ti, sys);
        return sys;
    }

    template<typename T>
    void SetSignature(uint64_t signature) {
        const std::type_index ti(typeid(T));
        assert(systems.find(ti) != systems.end() && "System used before registered.");
        signatures[ti] = signature;
    }

    void EntityDestroyed(EntityId entity) {
        for (auto const &pair : systems) {
            auto const &sys = pair.second;
            sys->entities.erase(entity);
        }
    }

    void EntitySignatureChanged(EntityId entity, uint64_t entitySignature) {
        for (auto const &pair : systems) {
            const std::type_index ti = pair.first;
            auto const &sys = pair.second;
            uint64_t systemSignature = signatures.count(ti) ? signatures.at(ti) : 0u;
            if ((entitySignature & systemSignature) == systemSignature) {
                sys->entities.insert(entity);
            } else {
                sys->entities.erase(entity);
            }
        }
    }

    template<typename T>
    std::shared_ptr<T> GetSystem() {
        const std::type_index ti(typeid(T));
        assert(systems.find(ti) != systems.end() && "System not registered.");
        return std::static_pointer_cast<T>(systems.at(ti));
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
    std::unordered_map<std::type_index, uint64_t> signatures;
};

} 