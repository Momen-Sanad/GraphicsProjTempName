#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>

class Shader {
private:
    GLuint program = 0;  // OpenGL shader program ID
    mutable std::unordered_map<std::string, GLint> uniformCache;  // Cache for uniform locations

public:
    Shader() = default;
    ~Shader();

    // Prevent copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    // Create shader from source code
    bool createFromSources(const std::string& vsSource, const std::string& fsSource);

    // Create shader from files
    bool createFromFiles(const std::string& vsPath, const std::string& fsPath);

    // Cleanup
    void destroy();

    // Use this shader program
    void use() const;

    // Get shader program ID
    GLuint getProgramID() const { return program; }

    // Get uniform location (cached)
    GLint getUniformLocation(const std::string& name) const;

    // Uniform setters for different types
    void set_bool(const std::string& name, bool value) const;
    void set_int(const std::string& name, int value) const;
    void set_float(const std::string& name, float value) const;
    void set_vec2(const std::string& name, const glm::vec2& value) const;
    void set_vec3(const std::string& name, const glm::vec3& value) const;
    void set_vec4(const std::string& name, const glm::vec4& value) const;
    void set_mat2(const std::string& name, const glm::mat2& value) const;
    void set_mat3(const std::string& name, const glm::mat3& value) const;
    void set_mat4(const std::string& name, const glm::mat4& value) const;
    void set_mat4_array(const std::string& name, const glm::mat4* values, int count) const;


private:
    // Helper functions
    static std::string readFile(const std::string& path);
    static GLuint compileShader(const std::string& source, GLenum type);
};