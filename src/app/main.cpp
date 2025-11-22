#include <glad/gl.h>
#include "engine/systems/RenderSystem.hpp"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include "engine/components/Camera.hpp"
#include "engine/components/MeshRenderer.hpp"

int main() {
    // Initialize GLFW and OpenGL (as before)
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Engine", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Set up ECS components
    Camera camera(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    MeshRenderer meshRenderer("path/to/mesh.obj", "path/to/material.mat");

    RenderSystem renderSystem;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera and rendering system
        renderSystem.render(camera, meshRenderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
