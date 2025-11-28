#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// ----------------------------------------------------
// Destructor
// ----------------------------------------------------
Shader::~Shader() {
    destroy();
}

// ----------------------------------------------------
// Move constructor
// ----------------------------------------------------
Shader::Shader(Shader&& other) noexcept {
    *this = std::move(other);
}

// ----------------------------------------------------
// Move assignment
// ----------------------------------------------------
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        destroy();
        program = other.program;
        uniformCache = std::move(other.uniformCache);

        other.program = 0;
    }
    return *this;
}

// ----------------------------------------------------
// File reading helper
// ----------------------------------------------------
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

// ----------------------------------------------------
// Shader compiler
// ----------------------------------------------------
GLuint Shader::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check compile status
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

// ----------------------------------------------------
// Create shader from sources
// ----------------------------------------------------
bool Shader::createFromSources(const std::string& vsSource,
                               const std::string& fsSource)
{
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

    // Check linking
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

// ----------------------------------------------------
// Create shader from file paths
// ----------------------------------------------------
bool Shader::createFromFiles(const std::string& vsPath,
                             const std::string& fsPath)
{
    std::string vs = readFile(vsPath);
    std::string fs = readFile(fsPath);

    if (vs.empty() || fs.empty())
        return false;

    return createFromSources(vs, fs);
}

// ----------------------------------------------------
// Cleanup
// ----------------------------------------------------
void Shader::destroy() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
    uniformCache.clear();
}

// ----------------------------------------------------
// Use program
// ----------------------------------------------------
void Shader::use() const {
    glUseProgram(program);
}

// ----------------------------------------------------
// Cached uniform locations
// ----------------------------------------------------
GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = uniformCache.find(name);
    if (it != uniformCache.end())
        return it->second;

    GLint loc = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = loc;
    return loc;
}
