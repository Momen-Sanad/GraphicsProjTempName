// src/engine/systems/RenderSystem.hpp

#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <GL/glew.h>
#include <vector>

class RenderSystem {
public:
    // Constructor and Destructor
    RenderSystem();
    ~RenderSystem();

    // Add a function to initialize the system
    void Init();
    
    // Render function that will be called in the main game loop
    void Render();
    
    // Function to clear the screen
    void ClearScreen();
    
private:
    GLuint shaderProgram;  // The current shader program
    GLuint vao;            // Vertex Array Object for the scene's objects
    GLuint vbo;            // Vertex Buffer Object for the vertices
    GLuint ebo;            // Element Buffer Object for the indices
};

#endif // RENDER_SYSTEM_HPP
