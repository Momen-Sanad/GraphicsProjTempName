#pragma once
#include <string>
#include <glad/gl.h>

GLuint LoadVert(const std::string& filename);
GLuint LoadFrag(const std::string& filename);
GLuint LinkProgram(GLuint vertShader, GLuint fragShader);
