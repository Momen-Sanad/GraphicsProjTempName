#pragma once

#include "AssetHandle.hpp"
#include "MaterialManager.hpp"
#include "ShaderManager.hpp"
#include <string>
#include <utility>

class AssetManager {
public:
    AssetManager() = default;

    ShaderManager& shaders() { return m_shaders; }
    const ShaderManager& shaders() const { return m_shaders; }

    MaterialManager& materials() { return m_materials; }
    const MaterialManager& materials() const { return m_materials; }

    AssetHandle<Shader> loadShader(const std::string& name,
                                   const std::string& vsPath,
                                   const std::string& fsPath);

    AssetHandle<Material> getMaterial(const std::string& name) const;

    template <typename T, typename... Args>
    AssetHandle<T> createMaterial(const std::string& name, Args&&... args) {
        return AssetHandle<T>(m_materials.create<T>(name, std::forward<Args>(args)...));
    }

private:
    ShaderManager m_shaders;
    MaterialManager m_materials;
};
