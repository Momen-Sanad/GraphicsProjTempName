#pragma once
#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif

#include "../animations/Skeleton.hpp"
#include <glm/glm.hpp>
#include <span>
#include <vector>
#include <cstdint>

// Color structure
struct Color {
    uint8_t r, g, b, a;
};

#define WHITE Color{ 255, 255, 255, 255 }
#define BLACK Color{   0,   0,   0, 255 }

// Vertex structure
struct Vertex
{
    glm::vec3 position;
    Color color;
    glm::vec2 tex_coord = {0.0f, 0.0f};
    glm::vec3 normal = {0.0f, 0.0f, 0.0f};
    bool operator==(const Vertex& other) const;
    void operator=(const Vertex& other);
};

class Mesh
{
protected:
    std::vector<Vertex> verticies;  // Vertex data
    std::vector<uint16_t> indices;  // Index data
    size_t count;                   // Number of elements to draw

public:
    // Constructors
    Mesh();
    Mesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices);
    void create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices);

    // Destructor
    ~Mesh();
    void destroy();

    // Static methods to create primitive meshes
    static Mesh create_cuboid(glm::vec3 center = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f));
    static Mesh create_plane(glm::vec3 center = glm::vec3(0.0f), glm::vec2 size = glm::vec2(1.0f), glm::vec2 tiling = glm::vec2(1.0f));
    static Mesh create_sphere(glm::ivec2 segments = { 32, 16 }, glm::vec3 center = glm::vec3(0.0f), float radius = 1.0f, bool invert_winding = false);
    static Mesh create_cylinder(int segments = 32, glm::vec3 center = glm::vec3(0.0f), float height = 1.0f, float radius = 1.0f);

    // Setters for mesh data
    void set_vertices(const std::span<Vertex>& positions);
    void set_indices(const std::span<uint16_t>& indices);
    void set_positions(const std::span<glm::vec3>& positions);
    void set_color(const Color& color);
    void set_colors(const std::span<Color>& colors);
    void set_tex_coords(const std::span<glm::vec2>& tex_coords);

    // Indexed setters
    void set_position(size_t index, const glm::vec3& position);
    void set_color(size_t index, const Color& color);
    void set_tex_coord(size_t index, const glm::vec2& tex_coord);
    void set_index(size_t index, uint16_t value);

    // Add methods
    void add_vertex(const Vertex& vertex);
    void add_index(uint16_t index);

    // Reserve capacity
    void reserve_vertices(size_t count);
    void reserve_indices(size_t count);

    // Getters for mesh data
    std::vector<Vertex>& get_vertices() { return verticies; }
    const std::vector<Vertex>& get_vertices() const { return verticies; }

    std::vector<uint16_t>& get_indices() { return indices; }
    const std::vector<uint16_t>& get_indices() const { return indices; }

    // ============== GETTERS ==============
    size_t get_vertex_count() const { return verticies.size(); }
    size_t get_index_count() const { return indices.size(); }
    size_t get_count() const { return count; }

    std::vector<glm::vec3> get_positions() const;

    // Attribute getters
    glm::vec3 get_position(size_t index) const;
    Color get_color(size_t index) const;
    glm::vec2 get_tex_coord(size_t index) const;

    // Utility getters
    std::vector<glm::vec3> get_positions() const;
    std::vector<glm::vec2> get_tex_coords() const;

    // OpenGL data getters
    const void* get_vertex_data() const;
    size_t get_vertex_data_size() const;
    size_t get_vertex_stride() const;

};
