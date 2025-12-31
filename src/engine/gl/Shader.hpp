#pragma once
#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
private:
    GLuint program = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache;

public:
    Shader() = default;
    ~Shader();

    // No copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow move
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool createFromSources(const std::string& vsSource, const std::string& fsSource);
    bool createFromFiles(const std::string& vsPath, const std::string& fsPath);

    void destroy();

    void use() const;

    GLuint getProgram() const { return program; }

    GLint getUniformLocation(const std::string& name) const;

    // Set uniform values for various types
    template <typename T>
    void setUniform(const std::string& name, const T& value);

private:
    static GLuint compileShader(const std::string& source, GLenum type);
    static std::string readFile(const std::string& path);
};