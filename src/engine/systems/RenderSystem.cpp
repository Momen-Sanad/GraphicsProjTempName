#include "RenderSystem.hpp"

#include "../components/CameraUtils.hpp"
#include "../components/MeshRenderer.hpp"
#include "../assets/Material.hpp"
#include "../ecs/World.hpp"
#include "../gl/GpuMesh.hpp"
#include "../platform/Window.hpp"

#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif

RenderSystem::RenderSystem(World& world, Window& window)
    : m_world(world), m_window(window) {}

void RenderSystem::render(float) {
    int width = 0;
    int height = 0;
    m_window.get_framebuffer_size(width, height);

    glViewport(0, 0, width, height);
    glClearColor(0.08f, 0.08f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 viewProj = camera_view_projection_matrix(
        m_world.get_camera(),
        glm::vec2(static_cast<float>(width), static_cast<float>(height))
    );

    if (!m_world.components().hasStorage<MeshRenderer>()) {
        return;
    }

    const auto& renderers = m_world.components().storage<MeshRenderer>().all();
    for (const auto& [id, renderer] : renderers) {
        if (!m_world.isAlive(id)) {
            continue;
        }
        if (!renderer.mesh || !renderer.material) {
            continue;
        }

        Entity* entity = m_world.getEntity(id);
        if (!entity) {
            continue;
        }

        glm::mat4 model = entity->getWorldMatrix();
        glm::mat4 mvp = viewProj * model;

        renderer.material->setup();
        GLint loc = renderer.material->getUniform("MVP");
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, &mvp[0][0]);
        }

        renderer.mesh->draw();
    }
}
