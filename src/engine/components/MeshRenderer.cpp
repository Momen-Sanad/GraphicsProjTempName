#include "MeshRenderer.hpp"
#include <cstring>

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

        vao = other.vao;
        vbo = other.vbo;
        ebo = other.ebo;
        count = other.count;

        other.vao = 0;
        other.vbo = 0;
        other.ebo = 0;
        other.count = 0;
    }
    return *this;
}

// ------------------------------------------------------------
// Upload data to GPU
// ------------------------------------------------------------
void MeshRenderer::upload(const MeshData& data) {
    destroy();  // clear previous mesh if present

    // Create VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // ------------------------
    // VBO
    // ------------------------
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 data.vertexCount() * sizeof(Vertex),
                 data.vertices.data(),
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
                 data.indexCount() * sizeof(uint16_t),
                 data.indices.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    count = data.indexCount();
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

    vao = 0;
    vbo = 0;
    ebo = 0;
    count = 0;
}

MeshRenderer MeshRenderer::createCube() {
    std::vector<Vertex> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, {255,255,255,255}},
        {{ 0.5f, -0.5f,  0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f,  0.5f}, {255,255,255,255}},
        {{-0.5f,  0.5f,  0.5f}, {255,255,255,255}},

        // Back face
        {{ 0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{-0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{-0.5f,  0.5f, -0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f, -0.5f}, {255,255,255,255}},

        // Left face
        {{-0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{-0.5f, -0.5f,  0.5f}, {255,255,255,255}},
        {{-0.5f,  0.5f,  0.5f}, {255,255,255,255}},
        {{-0.5f,  0.5f, -0.5f}, {255,255,255,255}},

        // Right face
        {{ 0.5f, -0.5f,  0.5f}, {255,255,255,255}},
        {{ 0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f, -0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f,  0.5f}, {255,255,255,255}},

        // Top face
        {{-0.5f,  0.5f,  0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f,  0.5f}, {255,255,255,255}},
        {{ 0.5f,  0.5f, -0.5f}, {255,255,255,255}},
        {{-0.5f,  0.5f, -0.5f}, {255,255,255,255}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, -0.5f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, -0.5f,  0.5f}, {255,255,255,255}},
        {{-0.5f, -0.5f,  0.5f}, {255,255,255,255}}
    };

    std::vector<uint16_t> indices = {
        // Front
        0,1,2, 2,3,0,
        // Back
        4,5,6, 6,7,4,
        // Left
        8,9,10, 10,11,8,
        // Right
        12,13,14, 14,15,12,
        // Top
        16,17,18, 18,19,16,
        // Bottom
        20,21,22, 22,23,20
    };

    MeshRenderer mesh;
    mesh.upload(MeshData(vertices, indices));
    return mesh;
}


MeshRenderer MeshRenderer::createPlane() {
    std::vector<Vertex> vertices = {
        {{-0.5f, 0.0f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, 0.0f, -0.5f}, {255,255,255,255}},
        {{ 0.5f, 0.0f,  0.5f}, {255,255,255,255}},
        {{-0.5f, 0.0f,  0.5f}, {255,255,255,255}}
    };

    std::vector<uint16_t> indices = {0,1,2, 2,3,0};

    MeshRenderer mesh;
    mesh.upload(MeshData(vertices, indices));
    return mesh;
}

MeshRenderer MeshRenderer::createSphere(int segments, int rings) {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    for (int y = 0; y <= rings; y++) {
        float v = float(y) / float(rings);
        float theta = v * glm::pi<float>();

        for (int x = 0; x <= segments; x++) {
            float u = float(x) / float(segments);
            float phi = u * glm::two_pi<float>();

            float px = std::cos(phi) * std::sin(theta);
            float py = std::cos(theta);
            float pz = std::sin(phi) * std::sin(theta);

            vertices.push_back({{px, py, pz}, {255,255,255,255}});
        }
    }

    for (int y = 0; y < rings; y++) {
        for (int x = 0; x < segments; x++) {
            int i0 = y     * (segments+1) + x;
            int i1 = (y+1) * (segments+1) + x;

            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i0+1);

            indices.push_back(i0+1);
            indices.push_back(i1);
            indices.push_back(i1+1);
        }
    }

    MeshRenderer mesh;
    mesh.upload(MeshData(vertices, indices));
    return mesh;
}

MeshRenderer MeshRenderer::createCylinder(int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    // Top + bottom circles + side vertices
    for (int i = 0; i <= segments; i++) {
        float t = float(i) / segments * glm::two_pi<float>();
        float x = std::cos(t);
        float z = std::sin(t);

        // top
        vertices.push_back({{x, 1.f, z}, {255,255,255,255}});
        // bottom
        vertices.push_back({{x, 0.f, z}, {255,255,255,255}});
    }

    // Indices for side
    for (int i = 0; i < segments; i++) {
        int top0 = i * 2;
        int bot0 = i * 2 + 1;
        int top1 = (i+1) * 2;
        int bot1 = (i+1) * 2 + 1;

        indices.push_back(top0);
        indices.push_back(bot0);
        indices.push_back(top1);

        indices.push_back(top1);
        indices.push_back(bot0);
        indices.push_back(bot1);
    }

    MeshRenderer mesh;
    mesh.upload(MeshData(vertices, indices));
    return mesh;
}
