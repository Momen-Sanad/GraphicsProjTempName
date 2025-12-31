#include "MeshRenderer.hpp"

// ============================================================
// MESH RENDERER IMPLEMENTATION
// ============================================================

MeshRenderer::MeshRenderer() {}

MeshRenderer::~MeshRenderer() {
    destroy();
}

// ------------------------------------------------------------
// Move Constructor
// ------------------------------------------------------------
MeshRenderer::MeshRenderer(MeshRenderer&& other) noexcept {
    *this = std::move(other);
}

// ------------------------------------------------------------
// Move Assignment
// ------------------------------------------------------------
MeshRenderer& MeshRenderer::operator=(MeshRenderer&& other) noexcept {
    if (this != &other) {
        destroy();

        // Move resources
        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        count = other.count;

        // Nullify source
        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.count = 0;
    }
    return *this;
}

// ------------------------------------------------------------
// Upload Mesh to GPU
// ------------------------------------------------------------
void MeshRenderer::upload(const Mesh& mesh) {
    destroy();

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and upload VBO (vertex data)
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

    // Create and upload EBO (index data)
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
// Draw Mesh
// ------------------------------------------------------------
void MeshRenderer::draw() const {
    if (!vao) return;

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);
    glBindVertexArray(0);
}

// ------------------------------------------------------------
// Destroy OpenGL Resources
// ------------------------------------------------------------
void MeshRenderer::destroy() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    count = 0;
}

