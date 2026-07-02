#pragma once

#include "Material.hpp"
#include "../gl/Texture.hpp"
#include "../gl/Shader.hpp"

#include <glm/glm.hpp>

#include <memory>

class LitMaterial : public Material
{
private:
    std::shared_ptr<Texture> albedoMap;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> roughnessMap;
    std::shared_ptr<Texture> emissiveMap;
    std::shared_ptr<Texture> ambientOcclusionMap;
    glm::vec3 albedoFactor = glm::vec3(1.0f);
    glm::vec3 specularFactor = glm::vec3(0.04f);
    glm::vec3 emissiveFactor = glm::vec3(0.0f);
    float roughnessFactor = 0.5f;
    float ambientOcclusionFactor = 1.0f;

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
