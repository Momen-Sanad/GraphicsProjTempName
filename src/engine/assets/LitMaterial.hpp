#pragma once
#include "TexturedMaterial.hpp"

class LitMaterial : public TexturedMaterial
{
private:
    Texture* albedoMap = nullptr;
    Texture* specularMap = nullptr;
    Texture* roughnessMap = nullptr;
    Texture* emissiveMap = nullptr;
    Texture* ambientOcclusionMap = nullptr;

public:
    // Constructor with default values for the texture maps
    LitMaterial(std::shared_ptr<Shader> shader, 
                Texture* albedo = nullptr, 
                Texture* specular = nullptr, 
                Texture* roughness = nullptr, 
                Texture* emissive = nullptr, 
                Texture* ambientOcclusion = nullptr);

    // Setter functions for all the maps
    void setAlbedoMap(Texture* tex);
    void setSpecularMap(Texture* tex);
    void setRoughnessMap(Texture* tex);
    void setEmissiveMap(Texture* tex);
    void setAmbientOcclusionMap(Texture* tex);

    void setup() override;
};
