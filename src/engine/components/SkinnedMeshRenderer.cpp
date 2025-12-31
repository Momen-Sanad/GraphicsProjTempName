#include "SkinnedMeshRenderer.hpp"


SkinnedMeshRenderer::SkinnedMeshRenderer() : MeshRenderer() {}

SkinnedMeshRenderer::~SkinnedMeshRenderer() {
    destroy();
}

// ------------------------------------------------------------
// Move Constructor
// ------------------------------------------------------------
SkinnedMeshRenderer::SkinnedMeshRenderer(SkinnedMeshRenderer&& other) noexcept
    : MeshRenderer(std::move(other))
{
    skeleton_vbo = other.skeleton_vbo;
    other.skeleton_vbo = 0;
}

// ------------------------------------------------------------
// Move Assignment
// ------------------------------------------------------------
SkinnedMeshRenderer& SkinnedMeshRenderer::operator=(SkinnedMeshRenderer&& other) noexcept {
    if (this != &other) {
        MeshRenderer::operator=(std::move(other));

        skeleton_vbo = other.skeleton_vbo;
        other.skeleton_vbo = 0;
    }
    return *this;
}

// ------------------------------------------------------------
// Upload Skinned Mesh to GPU
// ------------------------------------------------------------
void SkinnedMeshRenderer::upload(const SkinnedMesh& mesh) 
{
    destroy();

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // ========================================
    // VBO for regular vertex data (position, color, tex_coord)
    // ========================================
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
        mesh.get_vertex_data_size(),
        mesh.get_vertex_data(),
        GL_STATIC_DRAW);

    size_t stride = mesh.get_vertex_stride();

    // Attribute 0: Position (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
        (void*)offsetof(Vertex, position));

    // Attribute 1: Color (4 unsigned bytes)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride,
        (void*)offsetof(Vertex, color));

    // Attribute 2: Texture Coordinates (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
        (void*)offsetof(Vertex, tex_coord));

    // ========================================
    // VBO for skeleton data (normals, bone weights, bone IDs)
    // ========================================
    if (mesh.has_skeleton_data()) {
        glGenBuffers(1, &skeleton_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, skeleton_vbo);
        glBufferData(GL_ARRAY_BUFFER,
            mesh.get_skeleton_data_size(),
            mesh.get_skeleton_data(),
            GL_STATIC_DRAW);

        size_t skel_stride = mesh.get_skeleton_stride();

        // Attribute 3: Normal (vec3)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, skel_stride,
            (void*)offsetof(SkeletonVertex, normal));

        // Attribute 4: Bone Weights (vec4)
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, skel_stride,
            (void*)offsetof(SkeletonVertex, bone_weights));

        // Attribute 5: Bone IDs (ivec4)
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, skel_stride,
            (void*)offsetof(SkeletonVertex, bone_ids));
    }

    // ========================================
    // EBO for index data
    // ========================================
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        mesh.get_index_count() * sizeof(uint16_t),
        mesh.get_indices().data(),
        GL_STATIC_DRAW);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    count = static_cast<GLsizei>(mesh.get_index_count());
}

// ------------------------------------------------------------
// Destroy OpenGL Resources (including skeleton VBO)
// ------------------------------------------------------------
void SkinnedMeshRenderer::destroy() {
    // Destroy base class resources
    MeshRenderer::destroy();

    // Destroy skeleton VBO
    if (skeleton_vbo) {
        glDeleteBuffers(1, &skeleton_vbo);
        skeleton_vbo = 0;
    }
}