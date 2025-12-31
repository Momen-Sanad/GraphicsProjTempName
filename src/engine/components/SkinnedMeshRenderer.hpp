// SkinnedMeshRenderer.hpp
#pragma once
#include "MeshRenderer.hpp"
#include "../gl/SkinnedMesh.hpp"


class SkinnedMeshRenderer : public MeshRenderer {
private:
    GLuint skeleton_vbo = 0;  // Vertex Buffer Object for skeleton data

public:
    SkinnedMeshRenderer();
    ~SkinnedMeshRenderer() override;

    // Move semantics
    SkinnedMeshRenderer(SkinnedMeshRenderer&& other) noexcept;
    SkinnedMeshRenderer& operator=(SkinnedMeshRenderer&& other) noexcept;

    // Upload skinned mesh data to GPU
    void upload(const SkinnedMesh& mesh);

    // Clean up OpenGL resources including skeleton data
    void destroy() override;

    // Getters
    GLuint get_skeleton_vbo() const { return skeleton_vbo; }
};
