#include "ShaderManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint ShaderManager::loadShader(const std::string& filePath, GLenum shaderType) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return 0;
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderCode = shaderStream.str();
    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint ShaderManager::loadProgram(const std::string& vsPath, const std::string& fsPath) {
    std::string programKey = vsPath + fsPath;
    if (shaderPrograms.find(programKey) != shaderPrograms.end()) {
        return shaderPrograms[programKey];
    }

    GLuint vertexShader = loadShader(vsPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShader(fsPath, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    shaderPrograms[programKey] = program;
    return program;
}
