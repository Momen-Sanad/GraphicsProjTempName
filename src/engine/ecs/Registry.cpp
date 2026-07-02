#include "Registry.hpp"

#include <algorithm>

namespace engine::ecs {

EntityId Registry::createEntity() {
    uint32_t index = 0;
    if (!freeList_.empty()) {
        index = freeList_.back();
        freeList_.pop_back();
        alive_[index] = true;
    } else {
        index = static_cast<uint32_t>(generations_.size());
        generations_.push_back(0);
        alive_.push_back(true);
        signatures_.emplace_back();
    }

    signatures_[index].clear();
    aliveCount_++;
    return EntityId{index, generations_[index]};
}

bool Registry::isAlive(EntityId id) const {
    return id.valid() &&
           id.index < generations_.size() &&
           alive_[id.index] &&
           generations_[id.index] == id.generation;
}

void Registry::destroyEntity(EntityId id) {
    if (!isAlive(id)) {
        return;
    }

    for (auto& [_, componentPool] : pools_) {
        componentPool->remove(id);
    }
    signatures_[id.index].clear();

    alive_[id.index] = false;
    generations_[id.index]++;
    freeList_.push_back(id.index);
    aliveCount_--;
}

void Registry::clear() {
    pools_.clear();
    generations_.clear();
    alive_.clear();
    freeList_.clear();
    signatures_.clear();
    aliveCount_ = 0;
}

const ComponentSignature* Registry::signature(EntityId id) const {
    if (!isAlive(id) || id.index >= signatures_.size()) {
        return nullptr;
    }
    return &signatures_[id.index];
}

void Registry::validateAlive(EntityId id) const {
    if (!isAlive(id)) {
        throw std::runtime_error("Registry access with dead or invalid EntityId");
    }
}

} // namespace engine::ecs
