#include "Skeleton.hpp"
#include <glm/gtc/matrix_transform.hpp>

Skeleton::Skeleton()
    : global_inverse_transform(1.0f)
{
}

int Skeleton::add_bone(const std::string& name, int parent_id, const glm::mat4& inverse_bind_pose) {
    Bone bone;
    bone.id = static_cast<int>(bones.size());
    bone.name = name;
    bone.parent_id = parent_id;
    bone.inverse_bind_pose = inverse_bind_pose;
    bone.local_transform = glm::mat4(1.0f);

    bones.push_back(bone);
    bone_name_to_id[name] = bone.id;

    return bone.id;
}

int Skeleton::get_bone_id(const std::string& name) const {
    auto it = bone_name_to_id.find(name);
    if (it != bone_name_to_id.end()) {
        return it->second;
    }
    return -1;
}

bool Skeleton::has_bone(const std::string& name) const {
    return bone_name_to_id.find(name) != bone_name_to_id.end();
}

void Skeleton::calculate_bone_matrices(
    const std::vector<glm::mat4>& local_transforms,
    std::vector<glm::mat4>& bone_matrices
) const {
    bone_matrices.resize(bones.size());
    std::vector<glm::mat4> global_transforms(bones.size());

    // Calculate global transforms by traversing the hierarchy
    for (size_t i = 0; i < bones.size(); ++i) {
        const Bone& bone = bones[i];

        if (bone.parent_id < 0) {
            // Root bone
            global_transforms[i] = local_transforms[i];
        }
        else {
            // Child bone
            global_transforms[i] = global_transforms[bone.parent_id] * local_transforms[i];
        }

        // Calculate final bone matrix: GlobalInverse * GlobalTransform * InverseBindPose
        bone_matrices[i] = global_inverse_transform * global_transforms[i] * bone.inverse_bind_pose;
    }
}