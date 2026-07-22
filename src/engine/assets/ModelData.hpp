#pragma once

#include "../animations/AnimationClip.hpp"
#include "../animations/Skeleton.hpp"
#include "../gl/SkinnedMesh.hpp"
#include "../gl/Texture.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

struct ImportedModelNode {
    std::string name;
    glm::mat4 localTransform = glm::mat4(1.0f);
    int meshIndex = -1;
    int skinIndex = -1;
    int parentIndex = -1;
    std::vector<int> children;
};

struct ImportedModelScene {
    std::string name;
    std::vector<int> rootNodes;
};

struct ModelData {
    std::shared_ptr<Skeleton> skeleton;
    std::vector<std::shared_ptr<AnimationClip>> animations;
    std::vector<SkinnedMesh> meshes;
    std::vector<int> meshMaterialIndices;
    std::vector<std::vector<int>> meshPrimitiveIndices;
    std::vector<std::string> materialNames;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<ImportedModelNode> nodes;
    std::vector<ImportedModelScene> scenes;
    int defaultScene = -1;

    ModelData() = default;
    ModelData(ModelData&&) = default;
    ModelData& operator=(ModelData&&) = default;
    ModelData(const ModelData&) = default;
    ModelData& operator=(const ModelData&) = default;
};
