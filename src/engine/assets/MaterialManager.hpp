#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Material.hpp"
#include "TintedMaterial.hpp"

// MaterialManager class: Manages a collection of materials in memory.
class MaterialManager {
private:
    // Map that stores materials by their name as a key
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;

public:
    // Default constructor
    MaterialManager() = default;

    // Template function to create a new material of type T and store it with a specified name
    // Args are the constructor arguments for T 
    // (variadic templates -> accepts any amount of args and expands them upon usage)
    template<typename T, typename... Args>
    std::shared_ptr<T> create(const std::string& name, Args&&... args);

    // Function to retrieve a material by name from the materials map
    // Returns a shared pointer to the Material, or nullptr if not found
    std::shared_ptr<Material> get(const std::string& name) const;
};
