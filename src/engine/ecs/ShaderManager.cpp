#include "ShaderManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderManager& ShaderManager::get_instance() {
    static ShaderManager instance;
    return instance;
}

GLuint ShaderManager::load_shader(const std::string& vertex_path, const std::string& fragment_path) {
    // Load vertex and fragment shader source code
    std::ifstream vertex_file(vertex_path);
    std::ifstream fragment_file(fragment_path);

    std::stringstream vertex_stream, fragment_stream;
    vertex_stream << vertex_file.rdbuf();
    fragment_stream << fragment_file.rdbuf();

    std::string vertex_code = vertex_stream.str();
    std::string fragment_code = fragment_stream.str();

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment_shader);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Cache shader program
    shader_programs[vertex_path + fragment_path] = shader_program;

    // Cleanup
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

GLuint ShaderManager::get_shader_program(const std::string& name) {
    return shader_programs[name];
}
