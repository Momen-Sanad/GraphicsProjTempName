#pragma once

#include "Material.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Shader.hpp"

#include <memory>

class LitMaterial : public Material
{
private:
    std::shared_ptr<Texture> albedoMap;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> roughnessMap;
    std::shared_ptr<Texture> emissiveMap;
    std::shared_ptr<Texture> ambientOcclusionMap;

public:
    LitMaterial(std::shared_ptr<Shader> shader,
                std::shared_ptr<Texture> albedo           = nullptr,
                std::shared_ptr<Texture> specular         = nullptr,
                std::shared_ptr<Texture> roughness        = nullptr,
                std::shared_ptr<Texture> emissive         = nullptr,
                std::shared_ptr<Texture> ambientOcclusion = nullptr);

    // Map setters
    void setAlbedoMap(std::shared_ptr<Texture> tex);
    void setSpecularMap(std::shared_ptr<Texture> tex);
    void setRoughnessMap(std::shared_ptr<Texture> tex);
    void setEmissiveMap(std::shared_ptr<Texture> tex);
    void setAmbientOcclusionMap(std::shared_ptr<Texture> tex);

    // Upload textures & material state
    void setup() override;
};
