#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include "GLContext.hpp"

bool GLContext::init() {
    // glad/gl.h uses gladLoadGL with a loader function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return false;
    }
    return true;
}

void GLContext::enable_default_render_settings() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}