#include "MaterialManager.hpp"

// ------------------------------------------------------
// Get material by name
// ------------------------------------------------------
std::shared_ptr<Material> MaterialManager::get(const std::string& name) const {
    // Search for the material by its name in the materials map
    auto it = materials.find(name);

    // If found, return the shared pointer to the material
    if (it != materials.end())
        return it->second;

    // If the material is not found, return nullptr
    return nullptr;
}

