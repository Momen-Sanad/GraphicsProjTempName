#pragma once

#include "../animations/AnimationClip.hpp"
#include "../animations/Skeleton.hpp"
#include "../gl/SkinnedMesh.hpp"
#include "../gl/Texture.hpp"

#include <memory>
#include <vector>

struct ModelData {
    std::shared_ptr<Skeleton> skeleton;
    std::vector<std::shared_ptr<AnimationClip>> animations;
    std::vector<SkinnedMesh> meshes;
    std::vector<std::shared_ptr<Texture>> textures;

    ModelData() = default;
    ModelData(ModelData&&) = default;
    ModelData& operator=(ModelData&&) = default;
    ModelData(const ModelData&) = default;
    ModelData& operator=(const ModelData&) = default;
};
