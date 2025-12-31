#pragma once
#include <glm/glm.hpp>
#include <vector>
//#include "../gl/Mesh.hpp"
#include <string>
#include <unordered_map>


// Maximum number of bones that can influence a single vertex
constexpr int MAX_BONE_INFLUENCE = 4;

// Extended vertex structure for skinned meshes
struct SkeletonVertex
{
    glm::vec3 normal;
    glm::vec4 bone_weights;  // Weights for up to 4 bones
    glm::ivec4 bone_ids;     // Bone indices

    SkeletonVertex() : normal(0.0f, 1.0f, 0.0f),
        bone_weights(0.0f),
        bone_ids(-1) {
    }

    bool operator==(const SkeletonVertex& other) const {
        return normal == other.normal &&
            bone_weights == other.bone_weights &&
            bone_ids == other.bone_ids;
    }
};

// Maximum number of bones supported
constexpr int MAX_BONES = 100;


// Bone structure representing a joint in the skeleton
struct Bone {
    int id;
    std::string name;
    int parent_id;
    glm::mat4 inverse_bind_pose;  // Inverse of the bone's bind pose matrix
    glm::mat4 local_transform;     // Local transform relative to parent

    Bone()
        : id(-1)
        , parent_id(-1)
        , inverse_bind_pose(1.0f)
        , local_transform(1.0f)
    {
    }
};

// Skeleton class managing the bone hierarchy
class Skeleton 
{
private:
    std::vector<Bone> bones;
    std::unordered_map<std::string, int> bone_name_to_id;
    glm::mat4 global_inverse_transform;

public:
    Skeleton();

    // Add a bone to the skeleton
    int add_bone(const std::string& name, int parent_id, const glm::mat4& inverse_bind_pose);

    // Getters
    int get_bone_count() const { return static_cast<int>(bones.size()); }
    const Bone& get_bone(int id) const { return bones[id]; }
    Bone& get_bone(int id) { return bones[id]; }

    // Get bone ID by name
    int get_bone_id(const std::string& name) const;

    // Check if bone exists
    bool has_bone(const std::string& name) const;

    // Get all bones
    const std::vector<Bone>& get_bones() const { return bones; }
    std::vector<Bone>& get_bones() { return bones; }

    // Global inverse transform
    void set_global_inverse_transform(const glm::mat4& transform) {
        global_inverse_transform = transform;
    }
    const glm::mat4& get_global_inverse_transform() const {
        return global_inverse_transform;
    }

    // Calculate bone matrices for rendering
    void calculate_bone_matrices(
        const std::vector<glm::mat4>& local_transforms,
        std::vector<glm::mat4>& bone_matrices
    ) const;
};