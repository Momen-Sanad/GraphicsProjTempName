#include "Mesh.hpp"
#include <unordered_map>
#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cstddef>

namespace {
    // Helper: compute per-vertex normals from triangles (indices).
    // Accumulate face normals to each vertex then normalize.
    void compute_normals(std::vector<Vertex>& verts, const std::vector<uint16_t>& indices) {
        if (verts.empty() || indices.empty()) {
            // nothing to do
            return;
        }

        // zero out normals
        for (auto &v : verts) {
            v.normal = glm::vec3(0.0f);
        }

        // accumulate face normals
        size_t triCount = indices.size() / 3;
        for (size_t t = 0; t < triCount; ++t) {
            uint16_t i0 = indices[t*3 + 0];
            uint16_t i1 = indices[t*3 + 1];
            uint16_t i2 = indices[t*3 + 2];

            // guard against bad indices
            if (i0 >= verts.size() || i1 >= verts.size() || i2 >= verts.size()) continue;

            const glm::vec3 &p0 = verts[i0].position;
            const glm::vec3 &p1 = verts[i1].position;
            const glm::vec3 &p2 = verts[i2].position;

            glm::vec3 e1 = p1 - p0;
            glm::vec3 e2 = p2 - p0;

            glm::vec3 faceNormal = glm::cross(e1, e2);
            float len = glm::length(faceNormal);
            if (len > 1e-6f) {
                faceNormal /= len; // normalize
                verts[i0].normal += faceNormal;
                verts[i1].normal += faceNormal;
                verts[i2].normal += faceNormal;
            }
        }

        // normalize vertex normals; fallback to up if zero-length
        for (auto &v : verts) {
            float l = glm::length(v.normal);
            if (l > 1e-6f) {
                v.normal = v.normal / l;
            } else {
                // fallback normal (reasonable default)
                v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }
    }
} // anonymous namespace

Mesh::Mesh()
{
    count = 0;
}

Mesh::Mesh(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices)
{
    create(vertices, indices);
}

Mesh::~Mesh()
{
    destroy();
}

void Mesh::create(const std::span<Vertex>& vertices, const std::span<uint16_t>& indices)
{
    // Copy the vertices and indices into the member vectors.
    verticies.assign(vertices.begin(), vertices.end());
    this->indices.assign(indices.begin(), indices.end());
    
    // Compute normals from the geometry we just stored.
    compute_normals(verticies, this->indices);

    count = static_cast<GLsizei>(indices.size());
}

void Mesh::destroy() {
    count = 0;
}


Mesh Mesh::create_cuboid(glm::vec3 center, glm::vec3 size)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    glm::vec3 h = size * 0.5f;
    glm::vec3 c = center;

    // Front
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Back
    vertices.push_back({ .position = { c.x - h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Top
    vertices.push_back({ .position = { c.x - h.x,  c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x,  c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x,  c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x,  c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });

    // Bottom
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x - h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });

    // Right
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 0.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z - h.z }, .color = WHITE, .tex_coord = { 1.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y + h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 1.0f } });
    vertices.push_back({ .position = { c.x + h.x, c.y - h.y, c.z + h.z }, .color = WHITE, .tex_coord = { 0.0f, 0.0f } });

    // Left
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

    // compute normals
    compute_normals(vertices, indices);

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

    vertices.push_back({ .position = { c.x - h.x,  c.y, c.z + h.z }, .color = WHITE, .tex_coord = {     0.0f,     0.0f } });
    vertices.push_back({ .position = { c.x + h.x,  c.y, c.z + h.z }, .color = WHITE, .tex_coord = { tiling.x,     0.0f } });
    vertices.push_back({ .position = { c.x + h.x,  c.y, c.z - h.z }, .color = WHITE, .tex_coord = { tiling.x, tiling.y } });
    vertices.push_back({ .position = { c.x - h.x,  c.y, c.z - h.z }, .color = WHITE, .tex_coord = {     0.0f, tiling.y } });

    indices = {
        0, 1, 2, 2, 3, 0,
    };

    // plane normals (all up)
    compute_normals(vertices, indices);

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}

Mesh Mesh::create_sphere(glm::ivec2 segments, glm::vec3 center, float radius, bool invert_winding)
{
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    // populating the sphere vertices by looping over its longitude and latitude
    for (int lat = 0; lat <= segments.y; lat++)
    {
        float v = (float)lat / segments.y;
        float pitch = (v - 0.5f) * glm::pi<float>();
        float cos = glm::cos(pitch), sin = glm::sin(pitch);

        for (int lng = 0; lng <= segments.x; lng++)
        {
            float u = (float)lng / segments.x;
            float yaw = u * 2.0f * glm::pi<float>();
            glm::vec3 position = glm::vec3(cos * glm::cos(yaw), sin, cos * glm::sin(yaw)) * radius + center;
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

    if (invert_winding) {
        // Flip winding so the sphere faces inward (useful for sky spheres).
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            std::swap(indices[i + 1], indices[i + 2]);
        }
    }

    // compute normals (sphere uses position based normals)
    compute_normals(vertices, indices);

    Mesh mesh;
    mesh.create(vertices, indices);
    return mesh;
}


Mesh Mesh::create_cylinder(int segments, glm::vec3 center, float height, float radius)
{
    Mesh mesh;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> tex_coords;
    std::vector<uint16_t> indices;
    float halfH = height * 0.5f;

    // Side vertices
    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // bottom vertex
        positions.push_back(center + glm::vec3(x, -halfH, z));
        tex_coords.push_back(glm::vec2(t, 0.0f));

        // top vertex
        positions.push_back(center + glm::vec3(x, +halfH, z));
        tex_coords.push_back(glm::vec2(t, 1.0f));
    }

    // side indices
    for (int i = 0; i < segments; i++)
    {
        int base = i * 2;
        int i0 = base;
        int i1 = base + 1;
        int i2 = base + 2;
        int i3 = base + 3;

        // triangle 1
        indices.push_back(static_cast<uint16_t>(i0));
        indices.push_back(static_cast<uint16_t>(i2));
        indices.push_back(static_cast<uint16_t>(i1));

        // triangle 2
        indices.push_back(static_cast<uint16_t>(i1));
        indices.push_back(static_cast<uint16_t>(i2));
        indices.push_back(static_cast<uint16_t>(i3));
    }

    // Top cap
    int topCenterIndex = positions.size();
    positions.push_back(center + glm::vec3(0, +halfH, 0));
    tex_coords.push_back(glm::vec2(0.5f, 0.5f)); // center of cap

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        positions.push_back(center + glm::vec3(x, +halfH, z));

        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);
        tex_coords.push_back(glm::vec2(u, v));
    }

    for (int i = 0; i < segments; i++)
    {
        int i0 = topCenterIndex;
        int i1 = topCenterIndex + 1 + i;
        int i2 = topCenterIndex + 2 + i;

        indices.push_back(static_cast<uint16_t>(i0));
        indices.push_back(static_cast<uint16_t>(i1));
        indices.push_back(static_cast<uint16_t>(i2));
    }

    // Bottom cap
    int bottomCenterIndex = positions.size();
    positions.push_back(center + glm::vec3(0, -halfH, 0));
    tex_coords.push_back(glm::vec2(0.5f, 0.5f)); // center of cap

    for (int i = 0; i <= segments; i++)
    {
        float t = (float)i / segments;
        float angle = t * glm::two_pi<float>();
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        positions.push_back(center + glm::vec3(x, -halfH, z));

        float u = 0.5f + 0.5f * cos(angle);
        float v = 0.5f + 0.5f * sin(angle);
        tex_coords.push_back(glm::vec2(u, v));
    }

    for (int i = 0; i < segments; i++)
    {
        int i0 = bottomCenterIndex;
        int i1 = bottomCenterIndex + 2 + i;
        int i2 = bottomCenterIndex + 1 + i;

        indices.push_back(static_cast<uint16_t>(i0));
        indices.push_back(static_cast<uint16_t>(i1));
        indices.push_back(static_cast<uint16_t>(i2));
    }

    // Build mesh
    mesh.set_positions(positions);
    mesh.set_indices(indices);
    mesh.set_tex_coords(tex_coords);
    mesh.set_color(WHITE);

    // compute normals now because set_positions + set_indices were used
    compute_normals(mesh.verticies, mesh.indices);

    return mesh;
}

void Mesh::set_vertices(const std::span<Vertex>& positions)
{
    verticies.resize(positions.size());

    for (size_t i = 0; i < positions.size(); i++)
        verticies[i] = positions[i];

    // compute normals if we already have indices
    if (!indices.empty()) {
        compute_normals(verticies, indices);
    }
}

void Mesh::set_positions(const std::span<glm::vec3>& positions)
{
    verticies.resize(positions.size());

    for (size_t i = 0; i < positions.size(); i++)
        verticies[i].position = positions[i];

    // compute normals only if we have indices to form faces
    if (!indices.empty()) {
        compute_normals(verticies, indices);
    }
}

void Mesh::set_indices(const std::span<uint16_t>& indices)
{
    this->indices.resize(indices.size());

    for (size_t i = 0; i < indices.size(); i++)
        this->indices[i] = indices[i];

    count = static_cast<GLsizei>(indices.size());

    // compute normals now that indices are available
    if (!verticies.empty()) {
        compute_normals(verticies, this->indices);
    }
}

void Mesh::set_color(const Color& color)
{
    for (size_t i = 0; i < verticies.size(); i++)
        verticies[i].color = color;
}

void Mesh::set_colors(const std::span<Color>& colors)
{
    for (size_t i = 0; i < colors.size() && i < verticies.size(); i++)
        verticies[i].color = colors[i];
}

void Mesh::set_tex_coords(const std::span<glm::vec2>& tex_coords)
{
    for (size_t i = 0; i < tex_coords.size() && i < verticies.size(); i++)
        verticies[i].tex_coord = tex_coords[i];
}

std::vector<glm::vec3> Mesh::get_positions() const
{
    std::vector<glm::vec3> positions;

    positions.reserve(verticies.size());
    for (const auto& vertex : verticies)
        positions.push_back(vertex.position);

    return positions;
}

bool operator==(const Color& a, const Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool operator==(const Vertex& a, const Vertex& b) {
    return a.position == b.position && a.color == b.color && a.tex_coord == b.tex_coord;
}

bool Vertex::operator==(const Vertex& other) const
{
    return position == other.position && color.r == other.color.r && color.g == other.color.g &&
        color.b == other.color.b && color.a == other.color.a &&
        tex_coord == other.tex_coord;
}

void Vertex::operator=(const Vertex& other)
{
    position = other.position;
    color = other.color;
    tex_coord = other.tex_coord;
}

const Vertex& Mesh::get_vertex(size_t index) const
{
    return verticies.at(index);
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