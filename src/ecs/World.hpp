#pragma once

#include "Coordinator.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace ecs {

/* World is a convenience wrapper around Coordinator that adds name -> entity
   bookkeeping and parent/child relationships. This preserves
   a World + Entity-model while still using the manager-based
   Coordinator internally for components and systems. */

class World {
public:
    World() { coordinator.Init(); }

    EntityId CreateEntity(const std::string &name = "") {
        EntityId id = coordinator.CreateEntity();
        if (!name.empty())
            nameIndex.emplace(name, id);
        return id;
    }

    void DestroyEntity(EntityId id) {
        /* detach parent/children bookkeeping happens externally here; since
           this World doesn't keep explicit children lists (to keep it simple),
           user can implement relationship components if needed.
           Remove from name index
           NOTE: we keep it simple and remove any matching name entries.*/
        for (auto it = nameIndex.begin(); it != nameIndex.end();) {
            if (it->second == id) it = nameIndex.erase(it); else ++it;
        }
        coordinator.DestroyEntity(id);
    }

    EntityId FindByName(const std::string &name) const {
        auto it = nameIndex.find(name);
        if (it == nameIndex.end()) return 0;
        return it->second;
    }

    Coordinator& GetCoordinator() { return coordinator; }

private:
    Coordinator coordinator;
    std::unordered_multimap<std::string, EntityId> nameIndex;
};

}