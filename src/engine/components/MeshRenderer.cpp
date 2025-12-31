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
    destroy();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // ------------------------
    // VBO
    // ------------------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        data.get_vertex_count() * sizeof(Vertex),
        data.get_vertices().data(),
        GL_STATIC_DRAW
    );

    // ------------------------
    // Attribute 0 : POSITION
    // ------------------------
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, position)
    );

    // ------------------------
    // Attribute 1 : NORMAL  ✅ FIX
    // ------------------------
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, normal)
    );

    // ------------------------
    // Attribute 2 : TEXCOORD
    // ------------------------
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, tex_coord)
    );

    // ------------------------
    // Attribute 3 : COLOR
    // ------------------------
    if constexpr (std::is_same_v<decltype(Vertex::color), glm::u8vec4>) {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3, 4, GL_UNSIGNED_BYTE, GL_TRUE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, color)
        );
    } else {
        // Fallback: constant white if mesh has no colors
        glDisableVertexAttribArray(3);
        glVertexAttrib4f(3, 1.f, 1.f, 1.f, 1.f);
    }

    // ------------------------
    // EBO
    // ------------------------
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        data.get_index_count() * sizeof(uint16_t),
        data.get_indices().data(),
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);

    count = data.get_index_count();
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