#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>

// ------------------------------------------------------------
// Color & Vertex Structures
// ------------------------------------------------------------
struct Color {
    uint8_t r, g, b, a;
};

struct Vertex {
    glm::vec3 position;
    Color color;
};

// ------------------------------------------------------------
// CPU-side mesh data (no OpenGL here)
// ------------------------------------------------------------
class MeshData {
public:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    MeshData() = default;

    MeshData(std::vector<Vertex> v, std::vector<uint16_t> i)
        : vertices(std::move(v)), indices(std::move(i)) {}

    size_t vertexCount() const { return vertices.size(); }
    size_t indexCount()  const { return indices.size(); }
};

// ------------------------------------------------------------
// GPU-side mesh (VAO + VBO + EBO)
// ------------------------------------------------------------
class MeshRenderer {
private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei count = 0;

public:
    MeshRenderer();
    ~MeshRenderer();

    // No copying
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;

    // Move support
    MeshRenderer(MeshRenderer&& other) noexcept;
    MeshRenderer& operator=(MeshRenderer&& other) noexcept;

    void upload(const MeshData& data);
    void draw() const;
    void destroy();
};
