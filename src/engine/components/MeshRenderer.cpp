#include "MeshRenderer.hpp"

// ------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------
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

        vao   = other.vao;
        vbo   = other.vbo;
        ebo   = other.ebo;
        count = other.count;

        other.vao   = 0;
        other.vbo   = 0;
        other.ebo   = 0;
        other.count = 0;
    }
    return *this;
}

// ------------------------------------------------------------
// Upload data to GPU
// ------------------------------------------------------------
void MeshRenderer::upload(const Mesh& data) {
    destroy();  // Clear previous mesh if present

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // ------------------------
    // VBO
    // ------------------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 data.get_vertex_count() * sizeof(Vertex),  // Use getter for vertex count
                 data.get_vertices().data(),                // Use getter for vertices
                 GL_STATIC_DRAW);

    // Vertex attribute 0 -> position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    // Vertex attribute 1 -> color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, color)
    );

    // ------------------------
    // EBO
    // ------------------------
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data.get_index_count() * sizeof(uint16_t),  // Use getter for index count
                 data.get_indices().data(),                   // Use getter for indices
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    count = data.get_index_count();  // Update count using getter for index count
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
// Destroy OpenGL GPU Objects
// ------------------------------------------------------------
void MeshRenderer::destroy() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);

    vao   = 0;
    vbo   = 0;
    ebo   = 0;
    count = 0;
}