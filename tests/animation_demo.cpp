// ============================================================
// ANIMATION DEMO - Visual Integration Test
// Skeletal animation with GLTF model loading
// ============================================================

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>
#include <filesystem>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine/platform/Window.hpp"
#include "engine/gl/GLContext.hpp"
#include "engine/ecs/World.hpp"
#include "engine/ecs/CameraController.hpp"
#include "engine/ecs/ShaderManager.hpp"
#include "engine/ecs/Entity.hpp"
#include "engine/ecs/AnimationComponent.hpp"
#include "engine/components/MeshRenderer.hpp"
#include "engine/components/SkinnedMeshRenderer.hpp"
#include "engine/gl/Mesh.hpp"
#include "engine/gl/SkinnedMesh.hpp"
#include "engine/assets/MeshLoader.hpp"
#include "engine/assets/SkinnedMaterial.hpp"
#include "engine/assets/TextureLoader.hpp"
#include "engine/animations/Skeleton.hpp"
#include "engine/animations/Animator.hpp"
#include "engine/animations/AnimationClip.hpp"
#include <engine/assets/TintedMaterial.hpp>

#define WINDOW_W 1280
#define WINDOW_H 720

#ifndef SHADER_DIR
#define SHADER_DIR "../../shaders"
#endif

#ifdef SOURCE_DIR
static const char* SWORDMAN_GLTF_PATH = SOURCE_DIR "/tests/loading_files/the_swordman/scene.gltf";
#else
static const char* SWORDMAN_GLTF_PATH = "tests/loading_files/the_swordman/scene.gltf";
#endif

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Animation Demo - Swordman Model     " << std::endl;
    std::cout << "========================================" << std::endl;

    // Window + GL Context Setup
    Window window(WINDOW_W, WINDOW_H, "Animation Demo");
    if (!window.get_handle()) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    if (!GLContext::init()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        return 1;
    }
    GLContext::enable_default_render_settings();

    // ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load Shaders
    ShaderManager shaderManager;

    auto basicShader = shaderManager.loadShader("basic",
        std::string(SHADER_DIR) + "/blackToWhite.vert",
        std::string(SHADER_DIR) + "/blackToWhite.frag"
    );

    auto skinnedShader = shaderManager.loadShader("skinned",
        std::string(SHADER_DIR) + "/skinned.vert",
        std::string(SHADER_DIR) + "/skinned.frag"
    );

    if (!basicShader || !skinnedShader) {
        std::cerr << "Failed to load shaders" << std::endl;
        return 1;
    }

    // Create Materials
    TintedMaterial groundMaterial;
    groundMaterial.setShader(basicShader);
    groundMaterial.tint = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);

    TintedMaterial debugMaterial;
    debugMaterial.setShader(basicShader);
    debugMaterial.tint = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);

    SkinnedMaterial* characterMaterial = new SkinnedMaterial(skinnedShader, nullptr);
    characterMaterial->set_animated(true);

    // Create Meshes
    Mesh groundMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(500.0f, 0.5f, 500.0f));
    Mesh debugCubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(10.0f));

    MeshRenderer groundRenderer, debugCubeRenderer;
    groundRenderer.upload(groundMesh);
    debugCubeRenderer.upload(debugCubeMesh);

    // World + Camera Setup
    World world;

    // Camera positioned to see the swordman (model is ~170 units tall)
    world.get_camera().position = glm::vec3(200.f, 100.f, 200.f);
    world.get_camera().direction = glm::normalize(glm::vec3(0.f, 80.f, 0.f) - world.get_camera().position);
    world.get_camera().up = glm::vec3(0.f, 1.f, 0.f);
    world.get_camera().fov = glm::radians(60.0f);
    world.get_camera().near = 0.1f;
    world.get_camera().far = 2000.0f;

    // Scene Graph Setup
    Entity* root = world.createEntityWithParams(nullptr);

    Entity* ground = world.createEntityWithParams(root,
        { 0.f, -0.25f, 0.f }, glm::quat(), { 1.f, 1.f, 1.f },
        &groundRenderer, &groundMaterial);

    Entity* debugCube = world.createEntityWithParams(root,
        { 50.f, 10.f, 0.f }, glm::quat(), { 1.0f, 1.0f, 1.0f },
        &debugCubeRenderer, &debugMaterial);

    // Load Animated Character (Swordman)
    Entity* character = nullptr;
    AnimationComponent* animComponent = nullptr;
    std::shared_ptr<ModelData> characterModelData = nullptr;
    std::vector<SkinnedMeshRenderer*> characterRenderers;

    bool modelLoaded = false;
    if (std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        std::cout << "Loading animated model: " << SWORDMAN_GLTF_PATH << std::endl;

        ModelData* rawModel = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
        if (rawModel && rawModel->skeleton) {
            characterModelData = std::shared_ptr<ModelData>(rawModel);

            // Create character entity (scale 1.0 since model is already large)
            character = world.createEntityWithParams(root,
                { 0.f, 0.f, 0.f }, glm::quat(), { 1.0f, 1.0f, 1.0f },
                nullptr, nullptr);
            character->setName("Swordman");

            // Create skinned mesh renderers for each mesh
            for (auto& skinnedMesh : characterModelData->meshes) {
                SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
                renderer->upload(skinnedMesh);
                characterRenderers.push_back(renderer);
            }

            // Attach renderers and material to entity
            character->setSkinnedRenderers(characterRenderers);
            character->setSkinnedMaterial(characterMaterial);

            // Create and attach animation component
            animComponent = new AnimationComponent(characterModelData);
            character->addComponent(animComponent);

            // Play first animation
            if (animComponent->get_animation_count() > 0) {
                animComponent->play_animation(0, true);
                std::cout << "Playing animation: " << characterModelData->animations[0]->get_name() << std::endl;
            }

            // If model has textures, set the first one on the material
            if (!characterModelData->textures.empty()) {
                characterMaterial->setTexture(characterModelData->textures[0].get());
                std::cout << "  Textures: " << characterModelData->textures.size() << std::endl;
            }

            modelLoaded = true;
            std::cout << "Model loaded successfully!" << std::endl;
            std::cout << "  Bones: " << characterModelData->skeleton->get_bone_count() << std::endl;
            std::cout << "  Animations: " << characterModelData->animations.size() << std::endl;
            std::cout << "  Meshes: " << characterModelData->meshes.size() << std::endl;
        }
        else {
            std::cerr << "Failed to load model or model is missing skeleton/animations" << std::endl;
            delete rawModel;
        }
    }
    else {
        std::cerr << "Model file not found: " << SWORDMAN_GLTF_PATH << std::endl;
    }

    // Camera Controller Setup
    CameraController controller;
    controller.setup(window.get_handle(), &world.get_camera());

    // Animation Control Variables
    int currentAnimIndex = 0;
    float playbackSpeed = 1.0f;
    bool isPaused = false;

    // Timing
    float lastTime = static_cast<float>(glfwGetTime());

    // Main Loop
    while (!glfwWindowShouldClose(window.get_handle())) {
        window.poll_events();

        float time = static_cast<float>(glfwGetTime());
        float deltaTime = time - lastTime;
        lastTime = time;

        controller.update(deltaTime);

        // Rotate debug cube
        glm::quat cubeRot = glm::angleAxis(glm::radians(45.f) * deltaTime, glm::vec3(0.f, 1.f, 0.f));
        debugCube->setRotation(cubeRot * debugCube->getRotation());

        // Update animation components
        for (Entity* rootEntity : world.getEntityManager().getRoots()) {
            rootEntity->updateComponents(deltaTime);
        }

        // Rendering
        int w, h;
        window.get_framebuffer_size(w, h);
        if (w == 0 || h == 0) {
            window.swap_buffers();
            continue;
        }

        glViewport(0, 0, w, h);
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        glm::mat4 VP = world.get_camera().get_view_projection_matrix(glm::vec2(w, h));

        // Render all entities
        for (Entity* rootEntity : world.getEntityManager().getRoots()) {
            world.getEntityManager().renderEntityRecursive(rootEntity, VP);
        }

        // ImGui UI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Animation Controls");

        if (modelLoaded && animComponent) {
            ImGui::Text("Model: Swordman");
            ImGui::Text("Bones: %d", characterModelData->skeleton->get_bone_count());
            ImGui::Text("Current Time: %.2f", animComponent->get_animator().get_current_time());
            ImGui::Text("Playing: %s", animComponent->is_playing() ? "Yes" : "No");

            ImGui::Separator();

            ImGui::Text("Animations:");
            for (int i = 0; i < animComponent->get_animation_count(); i++) {
                const AnimationClip* clip = animComponent->get_animation(i);
                if (clip) {
                    bool isSelected = (i == currentAnimIndex);
                    if (ImGui::Selectable(clip->get_name().c_str(), isSelected)) {
                        currentAnimIndex = i;
                        animComponent->play_animation(i, true);
                    }
                }
            }

            ImGui::Separator();

            if (ImGui::SliderFloat("Speed", &playbackSpeed, 0.0f, 3.0f)) {
                animComponent->set_animation_speed(playbackSpeed);
            }

            if (ImGui::Button(isPaused ? "Resume" : "Pause")) {
                isPaused = !isPaused;
                if (isPaused) animComponent->pause_animation();
                else animComponent->resume_animation();
            }

            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                animComponent->stop_animation();
            }

            ImGui::SameLine();
            if (ImGui::Button("Restart")) {
                animComponent->play_animation(currentAnimIndex, true);
            }

            if (animComponent->get_animation(currentAnimIndex)) {
                float duration = animComponent->get_animation(currentAnimIndex)->get_duration();
                float currentTime = animComponent->get_animator().get_current_time();
                if (ImGui::SliderFloat("Timeline", &currentTime, 0.0f, duration)) {
                    animComponent->set_animation_time(currentTime);
                }
            }
        }
        else {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "No animated model loaded!");
            ImGui::Text("Place GLTF model at:");
            ImGui::TextWrapped("%s", SWORDMAN_GLTF_PATH);
        }

        ImGui::Separator();
        ImGui::Text("Camera: (%.1f, %.1f, %.1f)",
            world.get_camera().position.x,
            world.get_camera().position.y,
            world.get_camera().position.z);

        ImGui::Text("Controls:");
        ImGui::BulletText("WASD - Move camera");
        ImGui::BulletText("Mouse - Look around");
        ImGui::BulletText("Scroll - Zoom");

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.swap_buffers();
    }

    // Cleanup
    for (auto* renderer : characterRenderers) {
        renderer->destroy();
        delete renderer;
    }
    delete animComponent;
    delete characterMaterial;
    groundRenderer.destroy();
    debugCubeRenderer.destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}