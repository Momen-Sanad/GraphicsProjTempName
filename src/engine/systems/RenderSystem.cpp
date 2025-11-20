// src/engine/systems/RenderSystem.cpp

#include "RenderSystem.hpp"
#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

RenderSystem::RenderSystem() 
    : shaderProgram(0), vao(0), vbo(0), ebo(0) {}

RenderSystem::~RenderSystem() {
    // Cleanup the OpenGL objects
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void RenderSystem::Init() {
    // Example shader program loading (you can link shaders accordingly as you did in ShaderManager)
    ShaderManager shaderManager;
    GLuint vert = shaderManager.LoadShader("triangle.vert", GL_VERTEX_SHADER);
    GLuint frag = shaderManager.LoadShader("triangle.frag", GL_FRAGMENT_SHADER);
    shaderProgram = shaderManager.LinkProgram(vert, frag);

    // Create the vertex data (example for a triangle)
    GLfloat vertices[] = {
        // Positions        // Colors
        0.0f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Top vertex (Red)
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom-left vertex (Green)
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // Bottom-right vertex (Blue)
    };

    GLuint indices[] = {
        0, 1, 2  // Indices to form a single triangle
    };

    // Generate and bind a Vertex Array Object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Generate and bind a Vertex Buffer Object for the vertices
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind an Element Buffer Object for the indices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Position attribute
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Color attribute
    glEnableVertexAttribArray(1);

    // Unbind the VAO (good practice)
    glBindVertexArray(0);
}

void RenderSystem::Render() {
    ClearScreen();

    // Use the shader program
    glUseProgram(shaderProgram);

    // Bind the VAO for the current object
    glBindVertexArray(vao);

    // Draw the triangle using the element array buffer
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
}

void RenderSystem::ClearScreen() {
    // Clear the screen with a color (set in the main loop or here)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}