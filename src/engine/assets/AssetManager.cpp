#include "AssetManager.hpp"

AssetHandle<Shader> AssetManager::loadShader(const std::string& name,
                                              const std::string& vsPath,
                                              const std::string& fsPath) {
    return AssetHandle<Shader>(m_shaders.loadShader(name, vsPath, fsPath));
}

AssetHandle<Material> AssetManager::getMaterial(const std::string& name) const {
    return AssetHandle<Material>(m_materials.get(name));
}
