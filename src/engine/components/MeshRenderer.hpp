#ifndef MESH_RENDERER_HPP
#define MESH_RENDERER_HPP

#include <glm.hpp>
#include <string>

struct MeshRenderer {
    std::string meshFile;
    std::string materialFile;
    GLuint shaderProgram;

    MeshRenderer(const std::string& mesh, const std::string& material)
        : meshFile(mesh), materialFile(material), shaderProgram(0) {}
};

#endif // MESH_RENDERER_HPP
