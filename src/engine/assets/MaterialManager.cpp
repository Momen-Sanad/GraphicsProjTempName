#include "MaterialManager.hpp"

std::shared_ptr<Material> MaterialManager::get(const std::string& name) const {
    auto it = materials.find(name);
    if (it != materials.end()) {
        return it->second;
    }
    return nullptr;
}
