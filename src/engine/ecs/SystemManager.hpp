#pragma once

#include "Registry.hpp"
#include "System.hpp"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace engine::ecs {

class SystemManager {
public:
    template <typename T, typename... Args>
    T& add(Args&&... args) {
        static_assert(std::is_base_of_v<System, T>, "T must derive from engine::ecs::System");
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T& reference = *system;
        systems_.push_back(std::move(system));
        return reference;
    }

    void updateAll(Registry& registry, float deltaTime);
    void clear();
    size_t size() const { return systems_.size(); }

private:
    std::vector<std::unique_ptr<System>> systems_;
};

using SystemScheduler = SystemManager;

} // namespace engine::ecs
