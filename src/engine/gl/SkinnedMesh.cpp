#include "SkinnedMesh.hpp"

SkinnedMesh::SkinnedMesh() : Mesh()
{
}

SkinnedMesh::SkinnedMesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices,
    const std::span<SkeletonVertex>& skel_verticies)
{
    create(vertices, indices, skel_verticies);
}

void SkinnedMesh::create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices,
    const std::span<SkeletonVertex>& skel_verticies)
{
    Mesh::create(vertices, indices);
    this->skel_verticies.assign(skel_verticies.begin(), skel_verticies.end());
}

SkinnedMesh::~SkinnedMesh()
{
    destroy();
}

void SkinnedMesh::destroy()
{
    Mesh::destroy();
    skel_verticies.clear();
}

// ============================================================
// SKINNED MESH SETTERS
// ============================================================

void SkinnedMesh::set_skeleton_vertices(const std::span<SkeletonVertex>& skel_vertices)
{
    this->skel_verticies.assign(skel_vertices.begin(), skel_vertices.end());
}

void SkinnedMesh::set_normals(const std::span<glm::vec3>& normals)
{
    if (skel_verticies.size() != verticies.size()) {
        skel_verticies.resize(verticies.size());
    }

    size_t min_size = std::min(normals.size(), skel_verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        skel_verticies[i].normal = normals[i];
    }
}

void SkinnedMesh::set_bone_weights(const std::span<glm::vec4>& bone_weights)
{
    if (skel_verticies.size() != verticies.size()) {
        skel_verticies.resize(verticies.size());
    }

    size_t min_size = std::min(bone_weights.size(), skel_verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        skel_verticies[i].bone_weights = bone_weights[i];
    }
}

void SkinnedMesh::set_bone_ids(const std::span<glm::ivec4>& bone_ids)
{
    if (skel_verticies.size() != verticies.size()) {
        skel_verticies.resize(verticies.size());
    }

    size_t min_size = std::min(bone_ids.size(), skel_verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        skel_verticies[i].bone_ids = bone_ids[i];
    }
}

void SkinnedMesh::set_normal(size_t index, const glm::vec3& normal)
{
    if (index < verticies.size()) {
        if (skel_verticies.size() <= index) {
            skel_verticies.resize(index + 1);
        }
        skel_verticies[index].normal = normal;
    }
}

void SkinnedMesh::set_bone_weight(size_t index, const glm::vec4& bone_weight)
{
    if (index < verticies.size()) {
        if (skel_verticies.size() <= index) {
            skel_verticies.resize(index + 1);
        }
        skel_verticies[index].bone_weights = bone_weight;
    }
}

void SkinnedMesh::set_bone_id(size_t index, const glm::ivec4& bone_id)
{
    if (index < verticies.size()) {
        if (skel_verticies.size() <= index) {
            skel_verticies.resize(index + 1);
        }
        skel_verticies[index].bone_ids = bone_id;
    }
}

void SkinnedMesh::add_skeleton_vertex(const SkeletonVertex& vertex)
{
    skel_verticies.push_back(vertex);
}

void SkinnedMesh::reserve_skeleton_vertices(size_t count)
{
    skel_verticies.reserve(count);
}

// ============================================================
// SKINNED MESH GETTERS
// ============================================================

SkeletonVertex& SkinnedMesh::get_skeleton_vertex(size_t index)
{
    return skel_verticies.at(index);
}

const SkeletonVertex& SkinnedMesh::get_skeleton_vertex(size_t index) const
{
    return skel_verticies.at(index);
}

glm::vec3 SkinnedMesh::get_normal(size_t index) const
{
    if (index < skel_verticies.size()) {
        return skel_verticies[index].normal;
    }
    return glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec4 SkinnedMesh::get_bone_weight(size_t index) const
{
    if (index < skel_verticies.size()) {
        return skel_verticies[index].bone_weights;
    }
    return glm::vec4(0.0f);
}

glm::ivec4 SkinnedMesh::get_bone_id(size_t index) const
{
    if (index < skel_verticies.size()) {
        return skel_verticies[index].bone_ids;
    }
    return glm::ivec4(-1);
}

std::vector<glm::vec3> SkinnedMesh::get_normals() const
{
    std::vector<glm::vec3> normals;
    normals.reserve(skel_verticies.size());
    for (const auto& vertex : skel_verticies) {
        normals.push_back(vertex.normal);
    }
    return normals;
}

std::vector<glm::vec4> SkinnedMesh::get_bone_weights() const
{
    std::vector<glm::vec4> weights;
    weights.reserve(skel_verticies.size());
    for (const auto& vertex : skel_verticies) {
        weights.push_back(vertex.bone_weights);
    }
    return weights;
}

std::vector<glm::ivec4> SkinnedMesh::get_bone_ids() const
{
    std::vector<glm::ivec4> ids;
    ids.reserve(skel_verticies.size());
    for (const auto& vertex : skel_verticies) {
        ids.push_back(vertex.bone_ids);
    }
    return ids;
}

const void* SkinnedMesh::get_skeleton_data() const
{
    return skel_verticies.data();
}

size_t SkinnedMesh::get_skeleton_data_size() const
{
    return skel_verticies.size() * sizeof(SkeletonVertex);
}

size_t SkinnedMesh::get_skeleton_stride() const
{
    return sizeof(SkeletonVertex);
}