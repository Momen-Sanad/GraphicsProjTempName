#include "SystemManager.hpp"

namespace engine::ecs {

void SystemManager::updateAll(Registry& registry, float deltaTime) {
    for (const auto& system : systems_) {
        system->update(registry, deltaTime);
    }
}

void SystemManager::clear() {
    systems_.clear();
}

} // namespace engine::ecs
