#pragma once

#include "Material.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Shader.hpp"

#include <memory>

class LitMaterial : public Material
{
private:
    Texture* albedoMap           = nullptr;
    Texture* specularMap         = nullptr;
    Texture* roughnessMap        = nullptr;
    Texture* emissiveMap         = nullptr;
    Texture* ambientOcclusionMap = nullptr;

public:
    LitMaterial(std::shared_ptr<Shader> shader,
                Texture* albedo           = nullptr,
                Texture* specular         = nullptr,
                Texture* roughness        = nullptr,
                Texture* emissive         = nullptr,
                Texture* ambientOcclusion = nullptr);

    // Map setters
    void setAlbedoMap(Texture* tex);
    void setSpecularMap(Texture* tex);
    void setRoughnessMap(Texture* tex);
    void setEmissiveMap(Texture* tex);
    void setAmbientOcclusionMap(Texture* tex);

    // Upload textures & material state
    void setup() override;
};
