#include "AssetManager.hpp"
#include "TextureLoader.hpp"
#include "MeshLoader.hpp"
#include "../gl/Mesh.hpp"   // ensure we include it here as well
#include <iostream>
#include <memory>

// Constructor / Destructor / get_instance() as you already have...

// Generic template (optional to define here; keep your current implementation if you want it in the .cpp)
// If you leave the generic template defined here, other translation units cannot instantiate it
// unless you explicitly instantiate them here. That's okay if you only use explicit specializations below.

template <typename AssetType>
std::shared_ptr<AssetType> AssetManager::load_asset(const std::string& asset_path) {
    if (asset_cache.find(asset_path) != asset_cache.end()) {
        return std::static_pointer_cast<AssetType>(asset_cache[asset_path]);
    }

    std::shared_ptr<AssetType> asset = std::make_shared<AssetType>();
    asset->load(asset_path); // must compile only if AssetType is complete here
    asset_cache[asset_path] = asset;
    return asset;
}

// Specialization for Texture assets (example — update namespace if Texture is namespaced)
template<>
std::shared_ptr<Texture> AssetManager::load_asset<Texture>(const std::string& asset_path) {
    if (asset_cache.find(asset_path) != asset_cache.end()) {
        return std::static_pointer_cast<Texture>(asset_cache[asset_path]);
    }

    auto texture = std::make_shared<Texture>();
    texture->load(asset_path);
    asset_cache[asset_path] = texture;
    return texture;
}

// === IMPORTANT: Mesh specialization must use engine::gl::Mesh ===
template<>
std::shared_ptr<engine::gl::Mesh> AssetManager::load_asset<engine::gl::Mesh>(const std::string& asset_path) {
    if (asset_cache.find(asset_path) != asset_cache.end()) {
        return std::static_pointer_cast<engine::gl::Mesh>(asset_cache[asset_path]);
    }

    auto mesh = std::make_shared<engine::gl::Mesh>();
    mesh->load(asset_path); // Mesh::load is defined and Mesh is a complete type here
    asset_cache[asset_path] = mesh;
    return mesh;
}

// get_instance implementation...
AssetManager& AssetManager::get_instance() {
    static AssetManager instance;
    return instance;
}
