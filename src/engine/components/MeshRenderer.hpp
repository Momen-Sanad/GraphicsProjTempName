#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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


    /*
     * deletes the copy constructor, which means you cannot create a new MeshRenderer object by copying an existing one.
     * This prevents the class from being copied, ensuring that two objects 
     * don’t accidentally share the same resources or state.
    */

    // No copying
    MeshRenderer(const MeshRenderer&) = delete;
    MeshRenderer& operator=(const MeshRenderer&) = delete;
    
    
    /*
     *is the move constructor. It allows one MeshRenderer object to be moved to another, instead of copied.
     *This is important for performance because it transfers ownership of resources
     *(like memory or GPU buffers) rather than duplicating them.
     *
     *noexcept part indicates that this operation does not throw exceptions, 
     *which is often used for optimization 
     *for containers like std::vector to safely move elements
    */

    // Move support
    MeshRenderer(MeshRenderer&& other) noexcept;
    MeshRenderer& operator=(MeshRenderer&& other) noexcept;

    void upload(const MeshData& data);
    void draw() const;
    void destroy();


    // static factory functions
    static MeshRenderer createCube();
    static MeshRenderer createPlane();
    static MeshRenderer createSphere(int segments = 32, int rings = 16);
    static MeshRenderer createCylinder(int segments = 32);
};
