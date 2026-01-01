#pragma once
#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif
#include "../gl/Mesh.hpp"

// ------------------------------------------------------------
// GPU-side mesh renderer (VAO + VBO + EBO)
// Handles rendering of basic Mesh objects
// ------------------------------------------------------------
class MeshRenderer {
protected:
    GLuint vao = 0;  // Vertex Array Object
    GLuint vbo = 0;  // Vertex Buffer Object
    GLuint ebo = 0;  // Element Buffer Object
    GLsizei count = 0;  // Number of elements to render

public:
    MeshRenderer();
    virtual ~MeshRenderer();

    // Prevent copying
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;

    // Move semantics
    MeshRenderer(MeshRenderer&& other) noexcept;
    MeshRenderer& operator=(MeshRenderer&& other) noexcept;

    // Upload mesh data to GPU
    virtual void upload(const Mesh& mesh);

    // Render the mesh
    virtual void draw() const;

    // Clean up OpenGL resources
    virtual void destroy();

    // Getters
    GLuint get_vao() const { return vao; }
    GLuint get_vbo() const { return vbo; }
    GLuint get_ebo() const { return ebo; }
    GLsizei get_count() const { return count; }
    bool is_uploaded() const { return vao != 0; }
};

// ------------------------------------------------------------
// GPU-side skinned mesh renderer
// Handles rendering of SkinnedMesh objects with skeleton data
// ------------------------------------------------------------
