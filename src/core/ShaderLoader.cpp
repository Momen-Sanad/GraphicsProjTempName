#include "core/ShaderLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Helper function to read a file into a string
static std::string ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static GLuint CompileShader(GLenum type, const std::string& source, const std::string& name) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compilation failed (" << name << "):\n" << log << std::endl;
    } else {
        std::cout << "Shader compiled: " << name << std::endl;
    }

    return shader;
}

GLuint LoadVert(const std::string& filename) {
    std::string source = ReadFile(std::string(SHADER_DIR) + "/" + filename);
    return CompileShader(GL_VERTEX_SHADER, source, filename);
}

GLuint LoadFrag(const std::string& filename) {
    std::string source = ReadFile(std::string(SHADER_DIR) + "/" + filename);
    return CompileShader(GL_FRAGMENT_SHADER, source, filename);
}

GLuint LinkProgram(GLuint vertShader, GLuint fragShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Shader program link failed:\n" << log << std::endl;
    } else {
        std::cout << "Shader program linked successfully.\n";
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}
