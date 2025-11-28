#pragma once
#ifndef SHADERMANAGER
#define SHADERMANAGER
#include <glad/glad.h>
#include <string>
#include <unordered_map>

class ShaderManager {
public:
    GLuint load_shader(const std::string& path, GLenum shader_type);
    GLuint create_program(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
};
#endif