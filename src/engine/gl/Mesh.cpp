#include "Mesh.hpp"
#include "../assets/MeshLoader.hpp"
#include <iostream>

namespace engine {
namespace gl {

Mesh::Mesh()
    : vao(0), texture(0), element_count(0) {}

Mesh::~Mesh() = default;

void Mesh::load(const std::string& file_path) {
    bool ok = engine::assets::MeshLoader::load_obj(this, file_path);
    if (!ok) {
        std::cerr << "Mesh::load failed for " << file_path << "\n";
    }
}

GLuint Mesh::get_vao() const { return vao; }
GLuint Mesh::get_texture() const { return texture; }
GLuint Mesh::get_element_count() const { return element_count; }

void Mesh::set_vao(GLuint v) { vao = v; }
void Mesh::set_texture(GLuint t) { texture = t; }
void Mesh::set_element_count(GLuint c) { element_count = c; }

} // namespace gl
} // namespace engine
