#include "engine/assets/AssetManager.hpp"
#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/assets/TintedMaterial.hpp"
#include "engine/components/Light.hpp"
#include "engine/ecs/World.hpp"
#include "engine/gl/GLContext.hpp"
#include "engine/gl/Mesh.hpp"
#include "engine/platform/Window.hpp"
#include "engine/systems/AnimationSystem.hpp"
#include "engine/systems/RenderSystem.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#ifndef SHADER_DIR
#define SHADER_DIR "../shaders"
#endif

#ifdef SOURCE_DIR
constexpr const char* SWORDMAN_GLTF_PATH = SOURCE_DIR "/tests/loading_files/the_swordman/scene.gltf";
#else
constexpr const char* SWORDMAN_GLTF_PATH = "tests/loading_files/the_swordman/scene.gltf";
#endif

int main()
{
    Window window(960, 540, "Animation ECS Demo");
    if (!window.get_handle()) {
        return 1;
    }
    if (!GLContext::init()) {
        return 1;
    }
    GLContext::enable_default_render_settings();

    World world;
    world.get_camera().position = glm::vec3(0.0f, 2.0f, 5.0f);
    world.get_camera().direction = glm::normalize(glm::vec3(0.0f, -0.25f, -1.0f));
    world.get_camera().near = 0.1f;
    world.get_camera().far = 100.0f;

    world.lights().initUBO();
    world.lights().addLight(Light(
        LightType::DIRECTIONAL,
        glm::vec3(1.0f),
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(-0.25f, -1.0f, -0.25f),
        glm::radians(15.0f),
        glm::radians(30.0f),
        1.0f));

    const std::string shaderDir = SHADER_DIR;
    auto tintShader = world.assets().loadShader(
        "demo-tint",
        shaderDir + "/blackToWhite.vert",
        shaderDir + "/blackToWhite.frag");
    auto skinnedShader = world.assets().loadShader(
        "demo-skinned",
        shaderDir + "/skinned.vert",
        shaderDir + "/skinned.frag");

    if (!tintShader || !skinnedShader) {
        std::cerr << "Failed to load demo shaders.\n";
        world.shutdownGpuResources();
        return 1;
    }

    Mesh groundMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec2(6.0f));
    auto groundRenderer = world.assets().createMeshRenderer("demo-ground", groundMesh);
    auto groundMaterial = std::make_shared<TintedMaterial>();
    groundMaterial->setShader(tintShader);
    groundMaterial->tint = glm::vec4(0.25f, 0.5f, 0.25f, 1.0f);
    world.assets().registerMaterial("demo-ground", groundMaterial);
    world.createRenderable("Ground", groundRenderer, groundMaterial);

    if (std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        auto model = world.assets().loadModel(SWORDMAN_GLTF_PATH);
        if (model && !model->primitives.empty()) {
            auto renderers = world.assets().createSkinnedMeshRenderers("demo-swordman", *model);
            std::shared_ptr<Texture> texture;
            if (!model->textures.empty()) {
                texture = model->textures.front().texture;
            }
            auto material = std::make_shared<SkinnedMaterial>(skinnedShader, texture);
            material->set_animated(true);
            auto character = world.createSkinnedRenderable(
                "Swordman",
                renderers,
                material,
                model,
                engine::ecs::InvalidEntity,
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec3(0.01f));

            auto& animation = world.registry().emplace<engine::ecs::AnimatorData>(character);
            animation.model = model;
            animation.skinIndex = 0;
            animation.currentAnimation = 0;
            animation.playing = true;
            animation.loop = true;
        }
    } else {
        std::cerr << "Swordman model not found: " << SWORDMAN_GLTF_PATH << "\n";
    }

    AnimationSystem animationSystem;
    world.frameScheduler().reset();
    uint64_t frameIndex = 0;

    while (!window.should_close()) {
        window.poll_events();
        world.frameScheduler().tick();
        const float deltaTime = world.frameScheduler().delta_time();
        animationSystem.update(world.registry(), deltaTime);

        int width = 1;
        int height = 1;
        window.get_framebuffer_size(width, height);
        glViewport(0, 0, width, height);
        glClearColor(0.03f, 0.03f, 0.04f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const glm::vec2 viewport(
            width > 0 ? static_cast<float>(width) : 1.0f,
            height > 0 ? static_cast<float>(height) : 1.0f);
        RenderContext context;
        context.view = world.get_camera().get_view_matrix();
        context.projection = world.get_camera().get_projection_matrix(viewport);
        context.viewProj = context.projection * context.view;
        context.cameraPosition = world.get_camera().position;
        context.deltaTime = deltaTime;
        context.frameIndex = frameIndex++;
        world.renderer().render(world, context);

        window.swap_buffers();
    }

    world.shutdownGpuResources();
    return 0;
}
