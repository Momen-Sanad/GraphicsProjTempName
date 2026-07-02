#include "AssetManager.hpp"

#include "MeshLoader.hpp"
#include "TextureLoader.hpp"

#include <algorithm>
#include <iostream>

void AssetBounds::include(const glm::vec3& point)
{
    if (!valid) {
        min = point;
        max = point;
        valid = true;
        return;
    }

    min = glm::min(min, point);
    max = glm::max(max, point);
}

AssetHandle<Texture> AssetManager::loadTexture(const std::string& path)
{
    auto it = textures_.find(path);
    if (it != textures_.end()) {
        return it->second;
    }

    AssetHandle<Texture> texture = TextureLoader::readTexture(path);
    if (!texture) {
        return nullptr;
    }

    textures_[path] = texture;
    return texture;
}

AssetHandle<Mesh> AssetManager::loadMesh(const std::string& path)
{
    auto it = meshes_.find(path);
    if (it != meshes_.end()) {
        return it->second;
    }

    AssetHandle<Mesh> mesh = MeshLoader::readObjMesh(path.c_str());
    if (!mesh) {
        return nullptr;
    }

    meshes_[path] = mesh;
    return mesh;
}

AssetHandle<ModelAsset> AssetManager::loadModel(const std::string& path)
{
    auto it = models_.find(path);
    if (it != models_.end()) {
        return it->second;
    }

    auto modelData = MeshLoader::readGltfModel(path.c_str());
    if (!modelData) {
        return nullptr;
    }

    auto model = std::make_shared<ModelAsset>();
    model->path = path;
    model->animations = modelData->animations;

    if (modelData->skeleton) {
        SkinAsset skin;
        skin.name = "default";
        skin.skeleton = modelData->skeleton;
        const int boneCount = modelData->skeleton->get_bone_count();
        skin.joints.reserve(static_cast<size_t>(boneCount));
        for (int boneIndex = 0; boneIndex < boneCount; ++boneIndex) {
            skin.joints.push_back(boneIndex);
        }
        model->skins.push_back(std::move(skin));
    }

    for (auto& texture : modelData->textures) {
        model->textures.push_back({ {}, texture });
    }

    const int defaultSkinIndex = model->skins.empty() ? -1 : 0;
    for (const SkinnedMesh& source_mesh : modelData->meshes) {
        MeshPrimitive primitive;
        primitive.mesh = std::make_shared<SkinnedMesh>(source_mesh);
        primitive.skinIndex = defaultSkinIndex;
        for (const Vertex& vertex : primitive.mesh->get_vertices()) {
            primitive.bounds.include(vertex.position);
            model->bounds.include(vertex.position);
        }

        const int primitiveIndex = static_cast<int>(model->primitives.size());
        ModelNode node;
        node.name = "primitive_" + std::to_string(primitiveIndex);
        node.primitiveIndex = primitiveIndex;
        node.skinIndex = primitive.skinIndex;

        model->primitives.push_back(std::move(primitive));
        model->nodes.push_back(std::move(node));
    }

    if (!model->nodes.empty()) {
        ModelScene scene;
        scene.name = "default";
        scene.rootNodes.reserve(model->nodes.size());
        for (size_t nodeIndex = 0; nodeIndex < model->nodes.size(); ++nodeIndex) {
            scene.rootNodes.push_back(static_cast<int>(nodeIndex));
        }
        model->scenes.push_back(std::move(scene));
        model->defaultScene = 0;
    }

    if (model->primitives.empty()) {
        model->diagnostics.push_back("Model loaded without mesh primitives.");
    }
    if (model->textures.empty()) {
        model->diagnostics.push_back("Model loaded without textures.");
    }

    models_[path] = model;
    return model;
}

AssetHandle<Shader> AssetManager::loadShader(
    const std::string& name,
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    auto it = shaders_.find(name);
    if (it != shaders_.end()) {
        return it->second;
    }

    auto shader = std::make_shared<Shader>();
    if (!shader->createFromFiles(vertexPath, fragmentPath)) {
        return nullptr;
    }

    shaders_[name] = shader;
    return shader;
}

void AssetManager::registerMaterial(const std::string& name, AssetHandle<Material> material)
{
    if (!material) {
        materials_.erase(name);
        return;
    }
    materials_[name] = std::move(material);
}

AssetHandle<Material> AssetManager::getMaterial(const std::string& name) const
{
    auto it = materials_.find(name);
    return it != materials_.end() ? it->second : nullptr;
}

AssetHandle<MeshRenderer> AssetManager::createMeshRenderer(const std::string& name, const Mesh& mesh)
{
    auto it = meshRenderers_.find(name);
    if (it != meshRenderers_.end()) {
        return it->second;
    }

    auto renderer = std::make_shared<MeshRenderer>();
    renderer->upload(mesh);
    meshRenderers_[name] = renderer;
    return renderer;
}

AssetHandle<MeshRenderer> AssetManager::loadMeshRenderer(const std::string& path)
{
    auto it = meshRenderers_.find(path);
    if (it != meshRenderers_.end()) {
        return it->second;
    }

    AssetHandle<Mesh> mesh = loadMesh(path);
    if (!mesh) {
        return nullptr;
    }

    auto renderer = std::make_shared<MeshRenderer>();
    renderer->upload(*mesh);
    meshRenderers_[path] = renderer;
    return renderer;
}

std::vector<AssetHandle<SkinnedMeshRenderer>> AssetManager::createSkinnedMeshRenderers(
    const std::string& name,
    const ModelAsset& model)
{
    auto it = skinnedMeshRenderers_.find(name);
    if (it != skinnedMeshRenderers_.end()) {
        return it->second;
    }

    std::vector<AssetHandle<SkinnedMeshRenderer>> renderers;
    for (const MeshPrimitive& primitive : model.primitives) {
        if (primitive.mesh) {
            auto renderer = std::make_shared<SkinnedMeshRenderer>();
            renderer->upload(*primitive.mesh);
            renderers.push_back(renderer);
        }
    }

    skinnedMeshRenderers_[name] = renderers;
    return renderers;
}

void AssetManager::clear()
{
    skinnedMeshRenderers_.clear();
    meshRenderers_.clear();
    materials_.clear();
    shaders_.clear();
    models_.clear();
    meshes_.clear();
    textures_.clear();
    TextureLoader::clear_cache();
}
