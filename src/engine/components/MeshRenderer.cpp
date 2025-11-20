#include "MeshRenderer.hpp"
#include "../assets/MeshLoader.hpp"
#include "../assets/AssetManager.hpp"
#include <iostream>

void MeshRenderer::initialize(Entity* entity) {
    // Assuming that each entity will have a mesh and texture associated with it.
    // You might want to modify this if you have more specific initialization logic.
    
    // For now, let's load a simple mesh and texture for the entity.
    // We'll load from some asset path (you can modify this to be more flexible later).
    std::shared_ptr<Mesh> mesh = AssetManager::get_instance().load_asset<Mesh>("assets/meshes/my_mesh.obj");
    if (!mesh) {
        std::cerr << "Error: Mesh failed to load!" << std::endl;
        return;
    }
    
    // Set up VAO, texture, and element count from the loaded mesh
    vao = mesh->get_vao();
    element_count = mesh->get_element_count();

    // Let's assume the mesh has a texture (this could be improved depending on how you want to handle textures).
    texture = AssetManager::get_instance().load_asset<Texture>("assets/textures/my_texture.png")->get_texture_id();

    // Initialize other necessary components or data as required for the MeshRenderer.
}

void MeshRenderer::update(float dt) {
    // In this function, we handle drawing the mesh, possibly with animation or dynamic updates.

    // Use the shader program for rendering
    // You would normally bind a shader program here. Assuming you have a shader program already loaded.
    GLuint shaderProgram = 0;  // Replace with your actual shader program handle
    glUseProgram(shaderProgram);

    // Bind the VAO (already set in initialize)
    glBindVertexArray(vao);

    // Bind the texture (if it exists)
    glBindTexture(GL_TEXTURE_2D, texture);

    // Draw the mesh using the element count (indices)
    glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);

    // Optionally: Handle post-render actions, such as unbinding VAO/texture or applying transformations.
}

GLuint MeshRenderer::get_vao() const {
    return vao;
}

GLuint MeshRenderer::get_texture() const {
    return texture;
}

GLuint MeshRenderer::get_element_count() const {
    return element_count;
}
