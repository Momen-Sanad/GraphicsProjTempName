#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Material.hpp"
#include "TintedMaterial.hpp"

class MaterialManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;

public:
    MaterialManager() = default;

    template<typename T, typename... Args>
    std::shared_ptr<T> create(const std::string& name, Args&&... args);

    std::shared_ptr<Material> get(const std::string& name) const;
};