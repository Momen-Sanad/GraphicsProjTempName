#include "LitMaterial.hpp"
#include <glad/glad.h>

#include <utility>

namespace {
void bind_map(
    const std::shared_ptr<Shader>& shader,
    const char* legacyName,
    const char* modernName,
    const char* legacyHasName,
    const char* modernHasName,
    const std::shared_ptr<Texture>& texture,
    int unit)
{
    if (texture) {
        texture->bind(unit);
    } else {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (glad_glBindSampler) {
            glBindSampler(unit, 0);
        }
    }

    shader->setUniform(legacyName, unit);
    shader->setUniform(modernName, unit);
    shader->setUniform(legacyHasName, texture != nullptr);
    shader->setUniform(modernHasName, texture != nullptr);
}
} // namespace

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

void LitMaterial::setAlbedoFactor(const glm::vec3& factor)              { albedoFactor = factor; }
void LitMaterial::setSpecularFactor(const glm::vec3& factor)            { specularFactor = factor; }
void LitMaterial::setEmissiveFactor(const glm::vec3& factor)            { emissiveFactor = factor; }
void LitMaterial::setRoughnessFactor(float factor)                      { roughnessFactor = factor; }
void LitMaterial::setAmbientOcclusionFactor(float factor)               { ambientOcclusionFactor = factor; }

// ---------------- setup ----------------

void LitMaterial::setup()
{
    if (!shader) return;

    shader->use();

    // Albedo
    bind_map(shader, "material.albedo_map", "u_material.albedoMap", "material.has_albedo_map", "u_material.hasAlbedoMap", albedoMap, 0);

    // Specular
    bind_map(shader, "material.specular_map", "u_material.specularMap", "material.has_specular_map", "u_material.hasSpecularMap", specularMap, 1);

    // Ambient Occlusion
    bind_map(shader, "material.ambient_occlusion_map", "u_material.aoMap", "material.has_ambient_occlusion_map", "u_material.hasAoMap", ambientOcclusionMap, 2);

    // Roughness
    bind_map(shader, "material.roughness_map", "u_material.roughnessMap", "material.has_roughness_map", "u_material.hasRoughnessMap", roughnessMap, 3);

    // Emissive
    bind_map(shader, "material.emissive_map", "u_material.emissionMap", "material.has_emissive_map", "u_material.hasEmissionMap", emissiveMap, 4);

    shader->setUniform("material.albedo_factor", albedoFactor);
    shader->setUniform("material.specular_factor", specularFactor);
    shader->setUniform("material.emissive_factor", emissiveFactor);
    shader->setUniform("material.roughness_factor", roughnessFactor);
    shader->setUniform("material.ambient_occlusion_factor", ambientOcclusionFactor);

    shader->setUniform("u_material.albedoFactor", albedoFactor);
    shader->setUniform("u_material.metallicMap", 1);
    shader->setUniform("u_material.hasMetallicMap", false);
    shader->setUniform("u_material.metallicFactor", 0.0f);
    shader->setUniform("u_material.roughnessFactor", roughnessFactor);
    shader->setUniform("u_material.aoFactor", ambientOcclusionFactor);
    shader->setUniform("u_material.emissionFactor", emissiveFactor);
}
