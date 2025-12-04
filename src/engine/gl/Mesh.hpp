#pragma once

#include <glad/gl.h>
#include <glm.hpp>
#include <span>
#include <vector>
#include <cstdint>

struct Color {
    uint8_t r, g, b, a;
};

#define WHITE Color{ 255, 255, 255, 255 }
#define BLACK Color{   0,   0,   0, 255 }

// This will be our vertex struct which contains a position, a color and a texture coordinate.
struct Vertex
{
    glm::vec3 position;
    Color color;
    glm::vec2 tex_coord;

    bool operator==(const Vertex& other) const;
    void operator=(const Vertex& other);
};

class Mesh
{
private:
    std::vector<Vertex> verticies; // vertex buffer object
    std::vector<uint16_t> indices; // element buffer object
    GLsizei count; // The number of vertices to draw (will be the number of elements in the element buffer)

public:
    // constructors
    Mesh();
    Mesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices);
    void create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices);

    // destructors
    ~Mesh();
    void destroy();

    // create primitive meshes
    static Mesh create_cuboid(glm::vec3 center = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));
    static Mesh create_plane(glm::vec3 center = glm::vec3(0.0f), glm::vec2 size = glm::vec2(1.0f), glm::vec2 tiling = glm::vec2(1.0f));
    static Mesh create_sphere(glm::ivec2 segments = { 32, 16 }, glm::vec3 center = glm::vec3(0.0f), float radius = 1.0f);
    static Mesh create_cylinder(int segments = 32, glm::vec3 center = glm::vec3(0.0f), float height = 1.0f, float radius = 1.0f);

    // setters
    void set_vertices(const std::span<Vertex>& positions);
    void set_indices(const std::span<uint16_t>& indices);
    void set_positions(const std::span<glm::vec3>& positions);
    void set_color(const Color& color);
    void set_colors(const std::span<Color>& colors);
    void set_tex_coords(const std::span<glm::vec2>& tex_coords);

    // getters
    GLsizei get_vert_count() const;
    std::vector<Vertex> get_vertices() const;
    std::vector<glm::vec3> get_positions() const;
    std::vector<uint16_t> get_indices() const;
};

