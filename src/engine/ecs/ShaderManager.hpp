#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <glad/gl.h>
#include <string>
#include <unordered_map>

class ShaderManager {
public:
    static ShaderManager& getInstance() {
        static ShaderManager instance;
        return instance;
    }

    GLuint loadShader(const std::string& filePath, GLenum shaderType);
    GLuint loadProgram(const std::string& vsPath, const std::string& fsPath);

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::unordered_map<std::string, GLuint> shaderPrograms;
};

#endif // SHADER_MANAGER_HPP
