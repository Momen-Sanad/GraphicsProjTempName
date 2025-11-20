#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>

class ShaderManager {
public:
    static ShaderManager& get_instance();
    GLuint load_shader(const std::string& vertex_path, const std::string& fragment_path);
    GLuint get_shader_program(const std::string& name);

private:
    ShaderManager();
    ~ShaderManager();

    std::unordered_map<std::string, GLuint> shader_programs;
};
