#pragma once

#include <string>
#include <memory>
#include <unordered_map>

// include the real Mesh declaration (namespaced)
#include "../gl/Mesh.hpp"

// If you also use Texture or other concrete asset types in header, include them here too:
// #include "Texture.hpp"

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    static AssetManager& get_instance();

    // generic template declaration (no body here; specializations are provided in .cpp)
    template<typename AssetType>
    std::shared_ptr<AssetType> load_asset(const std::string& asset_path);

private:
    // store as generic shared_ptr (you already used this approach)
    std::unordered_map<std::string, std::shared_ptr<void>> asset_cache;
};
