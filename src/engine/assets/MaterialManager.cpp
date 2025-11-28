#include "MaterialManager.hpp"

// ------------------------------------------------------
// Create a material of any type T with forwarding args
// ------------------------------------------------------
template<typename T, typename... Args>
std::shared_ptr<T> MaterialManager::create(const std::string& name, Args&&... args) {
    auto mat = std::make_shared<T>(std::forward<Args>(args)...);
    materials[name] = mat;
    return mat;
}

// ------------------------------------------------------
// Get material by name
// ------------------------------------------------------
std::shared_ptr<Material> MaterialManager::get(const std::string& name) const {
    auto it = materials.find(name);
    if (it != materials.end())
        return it->second;
    return nullptr;
}

// Explicit template instantiation so the linker doesn't cry
template std::shared_ptr<TintedMaterial> 
MaterialManager::create<TintedMaterial>(const std::string& name);