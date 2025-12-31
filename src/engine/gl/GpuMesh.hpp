#pragma once

#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif

#include "../gl/Mesh.hpp"
#include <cstring>

// ------------------------------------------------------------
// GPU-side mesh (VAO + VBO + EBO)
// ------------------------------------------------------------
class GpuMesh {
private:
    GLuint vao    = 0; // Vertex Array Object
    GLuint vbo    = 0; // Vertex Buffer Object
    GLuint ebo    = 0; // Element Buffer Object
    GLsizei count = 0; // Number of elements to render

public:
    GpuMesh();
    ~GpuMesh();

    // Prevent copying to avoid accidental resource sharing.
    GpuMesh(const GpuMesh&) = delete;
    GpuMesh& operator=(const GpuMesh&) = delete;

    // Move constructor and move assignment operator.
    // Allows efficient transfer of resources without unnecessary copies.
    GpuMesh(GpuMesh&& other) noexcept;
    GpuMesh& operator=(GpuMesh&& other) noexcept;

    // Upload mesh data to the GPU (VBO, EBO).
    void upload(const Mesh& mesh);  // Use the proper Mesh type here.

    // Render the mesh using the bound VAO/VBO/EBO.
    void draw() const;

    // Clean up OpenGL resources (VAO, VBO, EBO).
    void destroy();
};
