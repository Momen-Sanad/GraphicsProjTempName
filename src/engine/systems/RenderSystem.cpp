#include "RenderSystem.hpp"

#include "../assets/Material.hpp"
#include "../assets/SkinnedMaterial.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/SkinnedMeshRenderer.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"
#include "../ecs/World.hpp"
#include "../gl/Shader.hpp"
#include "LightSystem.hpp"
#include "TransformSystem.hpp"

#include <glm/gtc/matrix_inverse.hpp>

RenderSystem::RenderSystem(LightSystem* lightSystem)
    : lightSystem_(lightSystem)
{
}

void RenderSystem::setLightSystem(LightSystem* lightSystem)
{
    lightSystem_ = lightSystem;
}

void RenderSystem::setDebugMode(RenderDebugMode mode)
{
    debugMode_ = mode;
}

void RenderSystem::render(World& world, const RenderContext& context)
{
    render(world.registry(), context);
}

void RenderSystem::render(engine::ecs::Registry& registry, const RenderContext& context)
{
    if (lightSystem_) {
        lightSystem_->updateUBO();
    }

    TransformSystem::updateWorldTransforms(registry);

    registry.each<engine::ecs::Transform, engine::ecs::Renderable>(
        [this, &context](
            engine::ecs::EntityId,
            engine::ecs::Transform& transform,
            engine::ecs::Renderable& renderable) {
            drawStatic(renderable.meshRenderer, renderable.material, transform.worldMatrix, context);
        });

    registry.each<engine::ecs::Transform, engine::ecs::SkinnedRenderable>(
        [this, &context](
            engine::ecs::EntityId,
            engine::ecs::Transform& transform,
            engine::ecs::SkinnedRenderable& renderable) {
            drawSkinned(renderable.renderers, renderable.material, transform.worldMatrix, context);
        });
}

void RenderSystem::drawStatic(
    const std::shared_ptr<MeshRenderer>& mesh,
    const std::shared_ptr<Material>& material,
    const glm::mat4& model,
    const RenderContext& context)
{
    if (!mesh || !material) {
        return;
    }

    std::shared_ptr<Shader> shader = material->getShader();
    if (!shader) {
        return;
    }

    shader->use();
    material->setup();
    applyFrameUniforms(*shader, context);

    if (shader->hasUniform("viewProj"))
        shader->setUniform("viewProj", context.viewProj);
    if (shader->hasUniform("u_view"))
        shader->setUniform("u_view", context.view);
    if (shader->hasUniform("u_proj"))
        shader->setUniform("u_proj", context.projection);
    if (shader->hasUniform("model"))
        shader->setUniform("model", model);
    if (shader->hasUniform("u_model"))
        shader->setUniform("u_model", model);
    if (shader->hasUniform("normalMatrix")) {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setUniform("normalMatrix", normalMatrix);
    }
    if (shader->hasUniform("u_normalMatrix")) {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setUniform("u_normalMatrix", normalMatrix);
    }
    if (shader->hasUniform("MVP")) {
        shader->setUniform("MVP", context.viewProj * model);
    }

    if (lightSystem_) {
        lightSystem_->setupLightsInShader(shader);
    }

    mesh->draw();
}

void RenderSystem::drawSkinned(
    const std::vector<std::shared_ptr<SkinnedMeshRenderer>>& renderers,
    const std::shared_ptr<SkinnedMaterial>& material,
    const glm::mat4& model,
    const RenderContext& context)
{
    if (renderers.empty() || !material) {
        return;
    }

    std::shared_ptr<Shader> shader = material->getShader();
    if (!shader) {
        return;
    }

    GLboolean cullEnabled = glIsEnabled(GL_CULL_FACE);
    if (cullEnabled) {
        glDisable(GL_CULL_FACE);
    }

    shader->use();
    material->setup();
    applyFrameUniforms(*shader, context);

    if (shader->hasUniform("MVP"))
        shader->setUniform("MVP", context.viewProj * model);
    if (shader->hasUniform("model"))
        shader->setUniform("model", model);
    if (shader->hasUniform("u_model"))
        shader->setUniform("u_model", model);
    if (shader->hasUniform("normalMatrix")) {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setUniform("normalMatrix", normalMatrix);
    }
    if (shader->hasUniform("u_normalMatrix")) {
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        shader->setUniform("u_normalMatrix", normalMatrix);
    }
    if (shader->hasUniform("uIsAnimated"))
        shader->setUniform("uIsAnimated", true);

    if (lightSystem_) {
        lightSystem_->setupLightsInShader(shader);
    }

    for (const auto& renderer : renderers) {
        if (renderer) {
            renderer->draw();
        }
    }

    if (cullEnabled) {
        glEnable(GL_CULL_FACE);
    }
}

void RenderSystem::applyFrameUniforms(Shader& shader, const RenderContext& context)
{
    const int debugMode = static_cast<int>(context.debugMode);

    if (shader.hasUniform("camera_pos"))
        shader.setUniform("camera_pos", context.cameraPosition);
    if (shader.hasUniform("u_cameraPos"))
        shader.setUniform("u_cameraPos", context.cameraPosition);
    if (shader.hasUniform("ambient"))
        shader.setUniform("ambient", glm::vec3(0.05f));
    if (shader.hasUniform("u_debugMode"))
        shader.setUniform("u_debugMode", debugMode);
    if (shader.hasUniform("debugMode"))
        shader.setUniform("debugMode", debugMode);
}
