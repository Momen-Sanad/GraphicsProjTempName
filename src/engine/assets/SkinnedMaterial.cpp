#include "SkinnedMaterial.hpp"

SkinnedMaterial::SkinnedMaterial() : TexturedMaterial(), is_animated(false) {
    bone_matrices.resize(MAX_BONES, glm::mat4(1.0f));
}

SkinnedMaterial::SkinnedMaterial(std::shared_ptr<Shader> shader, Texture* tex)
    : TexturedMaterial(shader, tex), is_animated(false) {
    bone_matrices.resize(MAX_BONES, glm::mat4(1.0f));
}

void SkinnedMaterial::set_bone_matrices(const std::vector<glm::mat4>& matrices) {
    size_t count = std::min(matrices.size(), static_cast<size_t>(MAX_BONES));
    for (size_t i = 0; i < count; ++i) {
        bone_matrices[i] = matrices[i];
    }
    is_animated = !matrices.empty();
}

void SkinnedMaterial::setup() {
    // First, do the parent setup (textures, etc.)
    TexturedMaterial::setup();

    // Then set up bone matrices if animated
    if (is_animated && shader) {
        shader->use();

        // Set bone matrices as uniforms
        for (int i = 0; i < MAX_BONES; ++i) {
            std::string uniform_name = "uBoneMatrices[" + std::to_string(i) + "]";
            shader->set_mat4(uniform_name, bone_matrices[i]);
        }

        shader->set_int("uNumBones", static_cast<int>(bone_matrices.size()));
        shader->set_bool("uIsAnimated", true);
        
        // Set texture flag
        bool hasTexture = (getTexture() != nullptr);
        shader->set_bool("u_hasTexture", hasTexture);
    }
}

void SkinnedMaterial::set_bone_count(int count) {
    if (shader) {
        shader->use();
        shader->set_int("uNumActiveBones", std::min(count, MAX_BONES));
    }
}