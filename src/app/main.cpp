#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../engine/ecs/World.hpp"
#include "../engine/ecs/Entity.hpp"
#include "../engine/ecs/CameraController.hpp"
#include "../engine/ecs/ShaderManager.hpp"
// Include glad after including GLFW to avoid OpenGL header conflict

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Modular Game Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD to load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    ShaderManager shader_manager;
    GLuint shader_program = shader_manager.create_program("vertex_shader.glsl", "fragment_shader.glsl");

    Camera camera;
    camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
    camera.direction = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    CameraController camera_controller(window, &camera);
    World world;
    world.camera = &camera;

    // Add entities to the world, set up their components...

    while (!glfwWindowShouldClose(window)) {
        float delta_time = glfwGetTime();
        glfwPollEvents();

        camera_controller.update(delta_time);
        world.update(delta_time);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.render();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
