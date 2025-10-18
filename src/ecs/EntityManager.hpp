/* Creates/destroys entities, tracks which components they have. */
#pragma once

#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <string>
#include <memory>

namespace ecs {

using EntityId = uint32_t;

class EntityManager {
public:
    EntityManager() = default;

    EntityId CreateEntity() {
        EntityId id;
        if (!freeIds.empty())
            id = freeIds.front(); freeIds.pop();
        
        else
            id = nextId++;
        
        // initialize signature as empty
        signatures[id] = 0u;
        return id;
    }

    void DestroyEntity(EntityId entity) {
        signatures.erase(entity);
        freeIds.push(entity);
    }

    void SetSignature(EntityId entity, uint64_t signature) {
        signatures[entity] = signature;
    }

    uint64_t GetSignature(EntityId entity) const {
        auto it = signatures.find(entity);
        
        if (it == signatures.end())
            return 0;
        
        return it->second;
    }

    // Utility to iterate all existing entity ids
    std::vector<EntityId> GetAllEntities() const {
        std::vector<EntityId> out;
        out.reserve(signatures.size());
        for (auto it = signatures.begin(); it != signatures.end(); ++it) {
            out.push_back(it->first);
        }

        return out;
    }

private:
    EntityId nextId = 1; // start at 1 for easier debugging (0 = invalid)
    std::queue<EntityId> freeIds;
    std::unordered_map<EntityId, uint64_t> signatures; // bitset signature per entity
};

}