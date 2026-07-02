// SkinnedMaterial.hpp
#pragma once
#include "TexturedMaterial.hpp"
#include <glm/glm.hpp>
#include <vector>

class SkinnedMaterial : public TexturedMaterial 
{
private:
    // Bone matrices for skeletal animation
    std::vector<glm::mat4> bone_matrices;

    // Maximum bones (should match shader)
    static constexpr int MAX_BONES = 100;

    // Is this material currently using skeletal animation?
    bool is_animated = false;
    int active_bone_count = 0;

public:
    SkinnedMaterial();
    SkinnedMaterial(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> tex);

    // Bone matrix management
    void set_bone_matrices(const std::vector<glm::mat4>& matrices);
    const std::vector<glm::mat4>& get_bone_matrices() const { return bone_matrices; }
    int get_active_bone_count() const { return active_bone_count; }

    void set_animated(bool animated) { is_animated = animated; }
    bool get_is_animated() const { return is_animated; }

    // Override setup to also set bone matrices
    void setup() override;

    // Set the number of active bones (for optimization)
    void set_bone_count(int count);

private:
    // Helper to get/set skinned shader
    std::shared_ptr<Shader> get_skinned_shader() const;
};
