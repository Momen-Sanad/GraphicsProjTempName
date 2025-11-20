#include "MeshLoader.hpp"
#include "../gl/Mesh.hpp"

#include <glad/gl.h>

#include <glm/glm.hpp>       
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <tuple>
#include <cstring>


namespace engine {
namespace assets {

// Helper to split a string by delimiter
static inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) parts.push_back(item);
    return parts;
}

// Parse an index token like "v", "v/vt", "v//vn", "v/vt/vn"
static void parse_obj_face_token(const std::string& token, int& v, int& vt, int& vn) {
    v = vt = vn = 0;
    auto parts = split(token, '/');
    if (parts.size() >= 1 && !parts[0].empty()) v  = std::stoi(parts[0]);
    if (parts.size() >= 2 && !parts[1].empty()) vt = std::stoi(parts[1]);
    if (parts.size() >= 3 && !parts[2].empty()) vn = std::stoi(parts[2]);
}

bool MeshLoader::load_obj(MeshPtr mesh, const std::string& file_path) {
    std::ifstream in(file_path);
    if (!in.is_open()) {
        std::cerr << "MeshLoader: failed to open OBJ: " << file_path << "\n";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    // Interleaved vertex data: pos (3) | normal (3) | uv (2) = 8 floats per vertex
    std::vector<float> vertex_data;
    std::vector<unsigned int> indices;

    // Map from "v/vt/vn" string to index in vertex_data (to deduplicate)
    std::unordered_map<std::string, unsigned int> vertex_map;
    vertex_map.reserve(1024);

    std::string line;
    while (std::getline(in, line)) {
        if (line.size() < 2) continue;
        if (line[0] == '#') continue;

        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            float x,y,z; ss >> x >> y >> z;
            positions.emplace_back(x,y,z);
        } else if (prefix == "vt") {
            float u,v; ss >> u >> v;
            texcoords.emplace_back(u,v);
        } else if (prefix == "vn") {
            float x,y,z; ss >> x >> y >> z;
            normals.emplace_back(x,y,z);
        } else if (prefix == "f") {
            // faces: variable number of vertices (triangulate if needed)
            std::vector<std::string> face_tokens;
            std::string tok;
            while (ss >> tok) face_tokens.push_back(tok);

            // Triangulate polygon fan-style (v0, v{i-1}, v{i})
            for (size_t i = 1; i + 1 < face_tokens.size(); ++i) {
                std::string tokens[3] = { face_tokens[0], face_tokens[i], face_tokens[i+1] };
                for (int k = 0; k < 3; ++k) {
                    auto &t = tokens[k];
                    // key = token string itself (e.g., "12/3/4" or "3//1")
                    auto it = vertex_map.find(t);
                    if (it != vertex_map.end()) {
                        indices.push_back(it->second);
                        continue;
                    }

                    int vi=0, vti=0, vni=0;
                    parse_obj_face_token(t, vi, vti, vni);

                    // OBJ is 1-based. Convert to 0-based and clamp
                    glm::vec3 pos(0.0f);
                    glm::vec2 uv(0.0f);
                    glm::vec3 nrm(0.0f);
                    if (vi != 0) {
                        int idx = vi > 0 ? vi - 1 : (int)positions.size() + vi; // negative indices support
                        if (idx >= 0 && idx < (int)positions.size()) pos = positions[idx];
                    }
                    if (vti != 0) {
                        int idx = vti > 0 ? vti - 1 : (int)texcoords.size() + vti;
                        if (idx >= 0 && idx < (int)texcoords.size()) uv = texcoords[idx];
                    }
                    if (vni != 0) {
                        int idx = vni > 0 ? vni - 1 : (int)normals.size() + vni;
                        if (idx >= 0 && idx < (int)normals.size()) nrm = normals[idx];
                    }

                    unsigned int new_index = (unsigned int)(vertex_data.size() / 8u); // 8 floats per vertex
                    // append interleaved vertex
                    vertex_data.push_back(pos.x);
                    vertex_data.push_back(pos.y);
                    vertex_data.push_back(pos.z);
                    vertex_data.push_back(nrm.x);
                    vertex_data.push_back(nrm.y);
                    vertex_data.push_back(nrm.z);
                    vertex_data.push_back(uv.x);
                    vertex_data.push_back(uv.y);

                    vertex_map.emplace(t, new_index);
                    indices.push_back(new_index);
                }
            }
        }
    }

    in.close();

    if (vertex_data.empty() || indices.empty()) {
        std::cerr << "MeshLoader: OBJ produced no data: " << file_path << "\n";
        return false;
    }

    // Generate GL buffers and upload data
    GLuint vao = 0, vbo = 0, ebo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // layout: location 0 = position (3 floats)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

    // layout: location 1 = normal (3 floats)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // layout: location 2 = uv (2 floats)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    // set mesh data (texture left 0 for now)
    mesh->set_vao(vao);
    mesh->set_element_count((GLuint)indices.size());
    mesh->set_texture(0u);

    // optionally delete vbo/ebo later on mesh destruction (not handled here)
    // we keep them alive because VAO references them

    return true;
}

} // namespace assets
} // namespace engine