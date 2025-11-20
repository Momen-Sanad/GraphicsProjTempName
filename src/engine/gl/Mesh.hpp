#pragma once
#include <string>
#include <glad/gl.h>

namespace engine {
namespace gl {

class Mesh {
public:
    Mesh();
    ~Mesh();

    // Called by AssetManager: load the mesh file (delegates to MeshLoader)
    void load(const std::string& file_path);

    // getters
    GLuint get_vao() const;
    GLuint get_texture() const;
    GLuint get_element_count() const;

    // simple setters used by MeshLoader
    void set_vao(GLuint v);
    void set_texture(GLuint t);
    void set_element_count(GLuint c);

private:
    GLuint vao;
    GLuint texture;
    GLuint element_count;
};

} // namespace gl
} // namespace engine
