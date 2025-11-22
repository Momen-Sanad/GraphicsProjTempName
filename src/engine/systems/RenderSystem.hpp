#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "../ecs/ShaderManager.hpp"
#include "../components/Camera.hpp"
#include "../components/MeshRenderer.hpp"

class RenderSystem {
public:
    void render(const Camera& camera, const MeshRenderer& meshRenderer) {
        // Load the shader program dynamically
        GLuint program = ShaderManager::getInstance().loadProgram(meshRenderer.meshFile, meshRenderer.materialFile);
        if (program == 0) return;

        glUseProgram(program);

        // Set up uniform variables for the MVP matrix, camera position, etc.
        GLint mvpLocation = glGetUniformLocation(program, "MVP");
        GLint cameraPosLocation = glGetUniformLocation(program, "cameraPos");
        
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &camera.projectionMatrix[0][0]);
        glUniform3fv(cameraPosLocation, 1, &camera.position[0]);

        // TODO: Bind mesh data and draw geometry here using OpenGL
    }
};

#endif // RENDER_SYSTEM_HPP
