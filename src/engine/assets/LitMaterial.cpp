#include "LitMaterial.hpp"

LitMaterial::LitMaterial(std::shared_ptr<Shader> shader, 
                Texture* albedo, 
                Texture* specular, 
                Texture* roughness, 
                Texture* emissive,
                Texture* ambientOcclusion
                )
        : TexturedMaterial(shader, albedo)  // Call the base class constructor
        {
            // Initialize LitMaterial-specific properties (specular, roughness, etc.)
            LitMaterial::setAlbedoMap(albedo);
            LitMaterial::setSpecularMap(specular);
            LitMaterial::setRoughnessMap(roughness);
            LitMaterial::setEmissiveMap(emissive);
            LitMaterial::setAmbientOcclusionMap(ambientOcclusion);
            LitMaterial::setup();
        }

void LitMaterial::setAlbedoMap(Texture* tex) {
    albedoMap = tex;
}

void LitMaterial::setSpecularMap(Texture* tex) {
    specularMap = tex;
}

void LitMaterial::setRoughnessMap(Texture* tex) {
    roughnessMap = tex;
}

void LitMaterial::setEmissiveMap(Texture* tex) {
    emissiveMap = tex;
}

void LitMaterial::setAmbientOcclusionMap(Texture* tex) {
    ambientOcclusionMap = tex;
}

void LitMaterial::setup() {
    TexturedMaterial::setup();

    // Bind and send texture maps to the shader
    if (albedoMap) {
        albedoMap->bind(0);
        glUniform1i(getUniformLocation("material.albedo_map"), 0);
    }

    if (specularMap) {
        specularMap->bind(1);
        glUniform1i(getUniformLocation("material.specular_map"), 1);
    }

    if (roughnessMap) {
        roughnessMap->bind(2);
        glUniform1i(getUniformLocation("material.roughness_map"), 2);
    }

    if (emissiveMap) {
        emissiveMap->bind(3);
        glUniform1i(getUniformLocation("material.emissive_map"), 3);
    }

    if (ambientOcclusionMap) {
        ambientOcclusionMap->bind(4);
        glUniform1i(getUniformLocation("material.ambient_occlusion_map"), 4);
    }
}
