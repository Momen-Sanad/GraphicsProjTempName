#include "MaterialManager.hpp"

// ------------------------------------------------------
// Create a material of any type T with forwarding args
// ------------------------------------------------------
template<typename T, typename... Args>
std::shared_ptr<T> MaterialManager::create(const std::string& name, Args&&... args) {
    // Create a shared pointer to a new material of type T using the forwarded arguments
    auto mat = std::make_shared<T>(std::forward<Args>(args)...);

    // Store the material in the 'materials' map with the given name as the key
    materials[name] = mat;

    // Return the shared pointer to the newly created material
    return mat;
}

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

// Explicit template instantiation so the linker doesn't cry
template std::shared_ptr<TintedMaterial> 
MaterialManager::create<TintedMaterial>(const std::string& name);