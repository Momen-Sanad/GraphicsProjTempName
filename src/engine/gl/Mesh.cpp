#include "Mesh.hpp"
#include <glm/gtc/constants.hpp>

// ============================================================
// COMPARISON OPERATORS
// ============================================================

bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool operator==(const Vertex& a, const Vertex& b) {
    return a.position == b.position && a.color == b.color && a.tex_coord == b.tex_coord;
}

bool Vertex::operator==(const Vertex& other) const
{
    return position == other.position &&
        color.r == other.color.r && color.g == other.color.g &&
        color.b == other.color.b && color.a == other.color.a &&
        tex_coord == other.tex_coord;
}

// ============================================================
// MESH CLASS IMPLEMENTATION
// ============================================================

Mesh::Mesh() : count(0)
{
}

Mesh::Mesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices)
{
    create(vertices, indices);
}

void Mesh::create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices)
{
    this->verticies.assign(vertices.begin(), vertices.end());
    this->indices.assign(indices.begin(), indices.end());
    count = static_cast<GLsizei>(indices.size());
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::destroy()
{
    count = 0;
    verticies.clear();
    indices.clear();
}

// ============================================================
// MESH PRIMITIVE CREATION
// ============================================================

Mesh Mesh::create_cuboid(glm::vec3 center, glm::vec3 size)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    glm::vec3 h = size * 0.5f;
    glm::vec3 c = center;

    // Front face
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Back face
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Top face
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });

    // Bottom face
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Right face
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });

    // Left face
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });

    indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}

Mesh Mesh::create_plane(glm::vec3 center, glm::vec2 size, glm::vec2 tiling)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    glm::vec3 h = glm::vec3(size.x, 0.0f, size.y) * 0.5f;
    glm::vec3 c = center;

    vertices.push_back({ .position = { c.x - h.x, c.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y, c.z + h.z }, .color = WHITE, .tex_coord = { tiling.x, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y, c.z - h.z }, .color = WHITE, .tex_coord = { tiling.x, tiling.y } });
    vertices.push_back({ .position = { c.x - h.x, c.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, tiling.y } });

    indices = { 0, 1, 2, 2, 3, 0 };

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}

Mesh Mesh::create_sphere(glm::ivec2 segments, glm::vec3 center, float radius)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    for (int lat = 0; lat <= segments.y; lat++)
    {
        float v = (float)lat / segments.y;
        float pitch = (v - 0.5f) * glm::pi<float>();
        float cos_p = glm::cos(pitch);
        float sin_p = glm::sin(pitch);

        for (int lng = 0; lng <= segments.x; lng++)
        {
            float u = (float)lng / segments.x;
            float yaw = u * 2.0f * glm::pi<float>();
            glm::vec3 position = glm::vec3(cos_p * glm::cos(yaw), sin_p, cos_p * glm::sin(yaw)) * radius + center;
            glm::vec2 tex_coord = glm::vec2(u, v);

            vertices.push_back({ .position = position, .color = WHITE, .tex_coord = tex_coord });
        }
    }

    for (int lat = 1; lat <= segments.y; lat++)
    {
        int start = lat * (segments.x + 1);

        for (int lng = 1; lng <= segments.x; lng++)
        {
            int prev_lng = lng - 1;
            indices.push_back(lng + start);
            indices.push_back(lng + start - segments.x - 1);
            indices.push_back(prev_lng + start - segments.x - 1);
            indices.push_back(prev_lng + start - segments.x - 1);
            indices.push_back(prev_lng + start);
            indices.push_back(lng + start);
        }
    }

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}

Mesh Mesh::create_cylinder(int segments, glm::vec3 center, float height, float radius)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    float halfH = height * 0.5f;

    // Side vertices
    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        vertices.push_back({ .position = center + glm::vec3(x, -halfH, z), .color = WHITE, .tex_coord = {t, 0.0f} });
        vertices.push_back({ .position = center + glm::vec3(x, +halfH, z), .color = WHITE, .tex_coord = {t, 1.0f} });
    }

    // Side indices
    for (int i = 0; i < segments; i++)
    {
        int base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    // Top cap
    int topCenterIndex = vertices.size();
    vertices.push_back({ .position = center + glm::vec3(0, +halfH, 0), .color = WHITE, .tex_coord = {0.5f, 0.5f} });

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        vertices.push_back({ .position = center + glm::vec3(x, +halfH, z), .color = WHITE, .tex_coord = {u, v} });
    }

    for (int i = 0; i < segments; i++)
    {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + 1 + i);
        indices.push_back(topCenterIndex + 2 + i);
    }

    // Bottom cap
    int bottomCenterIndex = vertices.size();
    vertices.push_back({ .position = center + glm::vec3(0, -halfH, 0), .color = WHITE, .tex_coord = {0.5f, 0.5f} });

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);

        vertices.push_back({ .position = center + glm::vec3(x, -halfH, z), .color = WHITE, .tex_coord = {u, v} });
    }

    for (int i = 0; i < segments; i++)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + 2 + i);
        indices.push_back(bottomCenterIndex + 1 + i);
    }

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}

// ============================================================
// MESH SETTERS
// ============================================================

void Mesh::set_vertices(const std::span<Vertex>& vertices)
{
    this->verticies.assign(vertices.begin(), vertices.end());
}

void Mesh::set_indices(const std::span<uint16_t>& indices)
{
    this->indices.assign(indices.begin(), indices.end());
    count = static_cast<GLsizei>(indices.size());
}

void Mesh::set_positions(const std::span<glm::vec3>& positions)
{
    size_t min_size = std::min(positions.size(), verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        verticies[i].position = positions[i];
    }
}

void Mesh::set_color(const Color& color)
{
    for (auto& vertex : verticies) {
        vertex.color = color;
    }
}

void Mesh::set_colors(const std::span<Color>& colors)
{
    size_t min_size = std::min(colors.size(), verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        verticies[i].color = colors[i];
    }
}

void Mesh::set_tex_coords(const std::span<glm::vec2>& tex_coords)
{
    size_t min_size = std::min(tex_coords.size(), verticies.size());
    for (size_t i = 0; i < min_size; ++i) {
        verticies[i].tex_coord = tex_coords[i];
    }
}

void Mesh::set_position(size_t index, const glm::vec3& position)
{
    if (index < verticies.size()) {
        verticies[index].position = position;
    }
}

void Mesh::set_color(size_t index, const Color& color)
{
    if (index < verticies.size()) {
        verticies[index].color = color;
    }
}

void Mesh::set_tex_coord(size_t index, const glm::vec2& tex_coord)
{
    if (index < verticies.size()) {
        verticies[index].tex_coord = tex_coord;
    }
}

void Mesh::set_index(size_t index, uint16_t value)
{
    if (index < indices.size()) {
        indices[index] = value;
    }
}

void Mesh::add_vertex(const Vertex& vertex)
{
    verticies.push_back(vertex);
}

void Mesh::add_index(uint16_t index)
{
    indices.push_back(index);
    count++;
}

void Mesh::reserve_vertices(size_t count)
{
    verticies.reserve(count);
}

void Mesh::reserve_indices(size_t count)
{
    indices.reserve(count);
}

// ============================================================
// MESH GETTERS
// ============================================================

Vertex& Mesh::get_vertex(size_t index)
{
    return verticies.at(index);
}

const Vertex& Mesh::get_vertex(size_t index) const
{
    return verticies.at(index);
}

uint16_t Mesh::get_index(size_t index) const
{
    return indices.at(index);
}

glm::vec3 Mesh::get_position(size_t index) const
{
    return verticies.at(index).position;
}

Color Mesh::get_color(size_t index) const
{
    return verticies.at(index).color;
}

glm::vec2 Mesh::get_tex_coord(size_t index) const
{
    return verticies.at(index).tex_coord;
}

std::vector<glm::vec3> Mesh::get_positions() const
{
    std::vector<glm::vec3> positions;
    positions.reserve(verticies.size());
    for (const auto& vertex : verticies) {
        positions.push_back(vertex.position);
    }
    return positions;
}

std::vector<glm::vec2> Mesh::get_tex_coords() const
{
    std::vector<glm::vec2> tex_coords;
    tex_coords.reserve(verticies.size());
    for (const auto& vertex : verticies) {
        tex_coords.push_back(vertex.tex_coord);
    }
    return tex_coords;
}

const void* Mesh::get_vertex_data() const
{
    return verticies.data();
}

size_t Mesh::get_vertex_data_size() const
{
    return verticies.size() * sizeof(Vertex);
}

size_t Mesh::get_vertex_stride() const
{
    return sizeof(Vertex);
}