#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <GL/glew.h>
#include <string>

class ShaderManager {
public:
    GLuint LoadShader(const std::string& filePath, GLenum shaderType);
    GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);
};

#endif // SHADER_MANAGER_HPP
