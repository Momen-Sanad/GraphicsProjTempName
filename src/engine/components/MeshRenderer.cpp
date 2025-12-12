#include "MeshRenderer.hpp"

// ------------------------------------------------------------
// Constructor / Destructor
// ------------------------------------------------------------
MeshRenderer::MeshRenderer() {}

MeshRenderer::~MeshRenderer() {
    destroy();  // Clean up OpenGL resources on destruction
}

// ------------------------------------------------------------
// Move Constructor
// ------------------------------------------------------------
MeshRenderer::MeshRenderer(MeshRenderer&& other) noexcept {
    *this = std::move(other);  // Move the resources from another object
}

// ------------------------------------------------------------
// Move Assignment
// ------------------------------------------------------------
MeshRenderer& MeshRenderer::operator=(MeshRenderer&& other) noexcept {
    if (this != &other) {  // Check for self-assignment
        destroy();  // Destroy any current resources

        // Move resources from the other object
        vao   = other.vao;
        vbo   = other.vbo;
        ebo   = other.ebo;
        count = other.count;

        // Nullify the other object to prevent double freeing
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
    destroy();  // Clear previous mesh data if it exists

    // Create a new VAO (stores the state of the vertex input)
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // ------------------------
    // VBO (stores vertex data)
    // ------------------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 data.get_vertex_count() * sizeof(Vertex),  // Size of vertex data
                 data.get_vertices().data(),                // Vertex data
                 GL_STATIC_DRAW);                           // Static data, won't change frequently

    // Vertex attribute 0 -> position
    glEnableVertexAttribArray(0);  // Enable attribute at location 0
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),                     // Stride: size of a vertex
        (void*)offsetof(Vertex, position)   // Offset to position data in the vertex structure
    );

    // Vertex attribute 1 -> color
    glEnableVertexAttribArray(1);  // Enable attribute at location 1
    glVertexAttribPointer(
        1, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(Vertex),                      // Stride: size of a vertex
        (void*)offsetof(Vertex, color)       // Offset to color data in the vertex structure
    );

    /*
    Added this part to support texture coordinates in the mesh.
    */

    // Vertex attribute 2 -> texture coordinates
    glEnableVertexAttribArray(2);  // Enable attribute at location 2
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),                     // Stride: size of a vertex
        (void*)offsetof(Vertex, tex_coord)  // Offset to texture coordinates data in the vertex structure
    );

    // ------------------------
    // EBO (stores index data)
    // ------------------------
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 data.get_index_count() * sizeof(uint16_t),  // Size of index data
                 data.get_indices().data(),                   // Index data
                 GL_STATIC_DRAW);                              // Static data, won't change frequently

    // Unbind the VAO (reset the OpenGL state)
    glBindVertexArray(0);

    count = data.get_index_count();  // Update the count to the number of indices
}

// ------------------------------------------------------------
// Draw Mesh
// ------------------------------------------------------------
void MeshRenderer::draw() const {
    if (!vao) return;  // If no VAO exists, return early

    glBindVertexArray(vao);  // Bind the VAO to the OpenGL context
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, nullptr);  // Draw the mesh using indices
    glBindVertexArray(0);  // Unbind the VAO after drawing
}

// ------------------------------------------------------------
// Destroy OpenGL GPU Objects
// ------------------------------------------------------------
void MeshRenderer::destroy() {
    // Delete the OpenGL resources if they exist
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);

    vao   = 0;
    vbo   = 0;
    ebo   = 0;
    count = 0;  // Reset the count
}