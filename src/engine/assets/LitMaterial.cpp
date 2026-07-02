#include "LitMaterial.hpp"
#include <glad/glad.h>

#include <utility>

LitMaterial::LitMaterial(
    std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> albedo,
    std::shared_ptr<Texture> specular,
    std::shared_ptr<Texture> roughness,
    std::shared_ptr<Texture> emissive,
    std::shared_ptr<Texture> ambientOcclusion
)
    : Material()
{
    setShader(std::move(shader));

    albedoMap           = albedo;
    specularMap         = specular;
    roughnessMap        = roughness;
    emissiveMap         = emissive;
    ambientOcclusionMap = ambientOcclusion;
}

// ---------------- setters ----------------

void LitMaterial::setAlbedoMap(std::shared_ptr<Texture> tex)            { albedoMap = std::move(tex); }
void LitMaterial::setSpecularMap(std::shared_ptr<Texture> tex)          { specularMap = std::move(tex); }
void LitMaterial::setRoughnessMap(std::shared_ptr<Texture> tex)         { roughnessMap = std::move(tex); }
void LitMaterial::setEmissiveMap(std::shared_ptr<Texture> tex)          { emissiveMap = std::move(tex); }
void LitMaterial::setAmbientOcclusionMap(std::shared_ptr<Texture> tex)  { ambientOcclusionMap = std::move(tex); }

// ---------------- setup ----------------

void LitMaterial::setup()
{
    if (!shader) return;

    shader->use();

    // Albedo
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoMap ? albedoMap->get_id() : 0);
    shader->setUniform("material.albedo_map", 0);

    // Specular
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap ? specularMap->get_id() : 0);
    shader->setUniform("material.specular_map", 1);

    // Ambient Occlusion
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ambientOcclusionMap ? ambientOcclusionMap->get_id() : 0);
    shader->setUniform("material.ambient_occlusion_map", 2);

    // Roughness
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughnessMap ? roughnessMap->get_id() : 0);
    shader->setUniform("material.roughness_map", 3);

    // Emissive
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, emissiveMap ? emissiveMap->get_id() : 0);
    shader->setUniform("material.emissive_map", 4);
}
