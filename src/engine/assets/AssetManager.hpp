#pragma once

#include "../animations/AnimationClip.hpp"
#include "../animations/Skeleton.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/SkinnedMeshRenderer.hpp"
#include "../gl/Mesh.hpp"
#include "../gl/Shader.hpp"
#include "../gl/SkinnedMesh.hpp"
#include "../gl/Texture.hpp"
#include "Material.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T>
using AssetHandle = std::shared_ptr<T>;

struct AssetBounds {
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);
    bool valid = false;

    void include(const glm::vec3& point);
};

struct TextureAsset {
    std::string path;
    AssetHandle<Texture> texture;
};

struct MaterialAsset {
    std::string name;
    AssetHandle<Material> material;
};

struct MeshPrimitive {
    AssetHandle<SkinnedMesh> mesh;
    int materialIndex = -1;
    int skinIndex = -1;
    AssetBounds bounds;
};

struct SkinAsset {
    std::string name;
    AssetHandle<Skeleton> skeleton;
    std::vector<int> joints;
};

struct ModelNode {
    std::string name;
    glm::mat4 localTransform = glm::mat4(1.0f);
    std::vector<int> primitiveIndices;
    int skinIndex = -1;
    int parentIndex = -1;
    std::vector<int> children;
};

struct ModelScene {
    std::string name;
    std::vector<int> rootNodes;
};

struct ModelAsset {
    std::string path;
    std::vector<ModelScene> scenes;
    std::vector<ModelNode> nodes;
    int defaultScene = -1;
    std::vector<MeshPrimitive> primitives;
    std::vector<MaterialAsset> materials;
    std::vector<TextureAsset> textures;
    std::vector<SkinAsset> skins;
    std::vector<std::shared_ptr<AnimationClip>> animations;
    AssetBounds bounds;
    std::vector<std::string> diagnostics;
};

class AssetManager {
public:
    AssetHandle<Texture> loadTexture(const std::string& path);
    AssetHandle<Mesh> loadMesh(const std::string& path);
    AssetHandle<ModelAsset> loadModel(const std::string& path);
    AssetHandle<Shader> loadShader(
        const std::string& name,
        const std::string& vertexPath,
        const std::string& fragmentPath);

    void registerMaterial(const std::string& name, AssetHandle<Material> material);
    AssetHandle<Material> getMaterial(const std::string& name) const;

    AssetHandle<MeshRenderer> createMeshRenderer(const std::string& name, const Mesh& mesh);
    AssetHandle<MeshRenderer> loadMeshRenderer(const std::string& path);
    std::vector<AssetHandle<SkinnedMeshRenderer>> createSkinnedMeshRenderers(const std::string& name, const ModelAsset& model);

    void clear();

private:
    std::unordered_map<std::string, AssetHandle<Texture>> textures_;
    std::unordered_map<std::string, AssetHandle<Mesh>> meshes_;
    std::unordered_map<std::string, AssetHandle<ModelAsset>> models_;
    std::unordered_map<std::string, AssetHandle<Shader>> shaders_;
    std::unordered_map<std::string, AssetHandle<Material>> materials_;
    std::unordered_map<std::string, AssetHandle<MeshRenderer>> meshRenderers_;
    std::unordered_map<std::string, std::vector<AssetHandle<SkinnedMeshRenderer>>> skinnedMeshRenderers_;
};
