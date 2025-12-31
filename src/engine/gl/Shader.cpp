#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::~Shader() {
    destroy();
}

Shader::Shader(Shader&& other) noexcept {
    *this = std::move(other);
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        destroy();
        program = other.program;
        uniformCache = std::move(other.uniformCache);
        other.program = 0;
    }
    return *this;
}

std::string Shader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, ' ');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cerr << "Shader compilation failed:\n" << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::createFromSources(const std::string& vsSource, const std::string& fsSource) {
    destroy();

    GLuint vs = compileShader(vsSource, GL_VERTEX_SHADER);
    if (!vs) return false;

    GLuint fs = compileShader(fsSource, GL_FRAGMENT_SHADER);
    if (!fs) {
        glDeleteShader(vs);
        return false;
    }

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(logLength, ' ');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cerr << "Shader link failed:\n" << log << std::endl;
        glDeleteProgram(program);
        program = 0;
        return false;
    }

    return true;
}

bool Shader::createFromFiles(const std::string& vsPath, const std::string& fsPath) {
    std::string vs = readFile(vsPath);
    std::string fs = readFile(fsPath);

    if (vs.empty() || fs.empty())
        return false;

    return createFromSources(vs, fs);
}

void Shader::destroy() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
    uniformCache.clear();
}

void Shader::use() const {
    glUseProgram(program);
}

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
        return it->second;

    GLint loc = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = loc;
    return loc;
}

// ============== UNIFORM SETTERS ==============

void Shader::set_bool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::set_int(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::set_float(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::set_vec2(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::set_vec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::set_vec4(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::set_mat2(const std::string& name, const glm::mat2& value) const {
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat3(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set_mat4_array(const std::string& name, const glm::mat4* values, int count) const {
    glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, glm::value_ptr(values[0]));
}