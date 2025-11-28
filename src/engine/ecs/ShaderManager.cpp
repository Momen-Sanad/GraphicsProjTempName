#include "ShaderManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

GLuint ShaderManager::load_shader(const std::string& path, GLenum shader_type) {
    std::ifstream shader_file(path);
    if (!shader_file.is_open()) {
        std::cerr << "Error: Could not open shader file " << path << std::endl;
        return 0;
    }

    std::stringstream buffer;
    buffer << shader_file.rdbuf();
    std::string code = buffer.str();

    GLuint shader = glCreateShader(shader_type);
    const char* code_cstr = code.c_str();
    glShaderSource(shader, 1, &code_cstr, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        std::cerr << "Shader compilation error: " << info_log << std::endl;
        return 0;
    }

    return shader;
}

GLuint ShaderManager::create_program(const std::string& vertex_shader_path, const std::string& fragment_shader_path) {
    GLuint vertex_shader = load_shader(vertex_shader_path, GL_VERTEX_SHADER);
    GLuint fragment_shader = load_shader(fragment_shader_path, GL_FRAGMENT_SHADER);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    int success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
        std::cerr << "Program linking error: " << info_log << std::endl;
        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
