#pragma once
#include "Mesh.hpp"
#include <vector>

class SkinnedMesh : public Mesh
{
private:
    std::vector<SkeletonVertex> skel_verticies;  // Skeleton vertex data

public:
    // Constructors
    SkinnedMesh();
    SkinnedMesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices,
        const std::span<SkeletonVertex>& skel_verticies);
    void create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices,
        const std::span<SkeletonVertex>& skel_verticies);

    // Destructor
    ~SkinnedMesh() override;
    void destroy() override;

    // ============== SKELETON SETTERS ==============
    void set_skeleton_vertices(const std::span<SkeletonVertex>& skel_vertices);
    void set_normals(const std::span<glm::vec3>& normals);
    void set_bone_weights(const std::span<glm::vec4>& bone_weights);
    void set_bone_ids(const std::span<glm::ivec4>& bone_ids);

    // Indexed setters
    void set_normal(size_t index, const glm::vec3& normal);
    void set_bone_weight(size_t index, const glm::vec4& bone_weight);
    void set_bone_id(size_t index, const glm::ivec4& bone_id);

    // Add methods
    void add_skeleton_vertex(const SkeletonVertex& vertex);

    // Reserve capacity
    void reserve_skeleton_vertices(size_t count);

    // ============== SKELETON GETTERS ==============
    bool has_skeleton_data() const { return !skel_verticies.empty(); }
    size_t get_skeleton_vertex_count() const { return skel_verticies.size(); }

    std::vector<SkeletonVertex>& get_skeleton_vertices() { return skel_verticies; }
    const std::vector<SkeletonVertex>& get_skeleton_vertices() const { return skel_verticies; }

    // Indexed getters
    SkeletonVertex& get_skeleton_vertex(size_t index);
    const SkeletonVertex& get_skeleton_vertex(size_t index) const;
    glm::vec3 get_normal(size_t index) const;
    glm::vec4 get_bone_weight(size_t index) const;
    glm::ivec4 get_bone_id(size_t index) const;

    // Utility getters
    std::vector<glm::vec3> get_normals() const;
    std::vector<glm::vec4> get_bone_weights() const;
    std::vector<glm::ivec4> get_bone_ids() const;

    // OpenGL data getters
    const void* get_skeleton_data() const;
    size_t get_skeleton_data_size() const;
    size_t get_skeleton_stride() const;
};