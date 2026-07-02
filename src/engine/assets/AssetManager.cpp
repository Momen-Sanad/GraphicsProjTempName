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
    model->legacyModel = std::move(modelData);
    model->animations = model->legacyModel->animations;

    if (model->legacyModel->skeleton) {
        model->skins.push_back({ model->legacyModel->skeleton });
    }

    for (auto& texture : model->legacyModel->textures) {
        model->textures.push_back({ {}, texture });
    }

    for (const SkinnedMesh& source_mesh : model->legacyModel->meshes) {
        MeshPrimitive primitive;
        primitive.mesh = std::make_shared<SkinnedMesh>(source_mesh);
        for (const Vertex& vertex : primitive.mesh->get_vertices()) {
            primitive.bounds.include(vertex.position);
        }
        model->primitives.push_back(std::move(primitive));
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
    if (model.legacyModel) {
        for (const SkinnedMesh& mesh : model.legacyModel->meshes) {
            auto renderer = std::make_shared<SkinnedMeshRenderer>();
            renderer->upload(mesh);
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
