// GL and GLFW related headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <string>

// ImGui related headers (for GUI)
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Custom headers for engine-related functionality
#include "../engine/platform/Window.hpp"
#include "../engine/gl/GLContext.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/ecs/CameraController.hpp"
#include "../engine/ecs/ShaderManager.hpp"
#include "../engine/assets/MaterialManager.hpp"
#include "../engine/components/MeshRenderer.hpp"
#include "../engine/gl/Mesh.hpp"
#include "../engine/utils/Im_GUI_Inspector.hpp"
#include "../engine/assets/MeshLoader.hpp"
#include "../engine/assets/TexturedMaterial.hpp"
#include "../engine/assets/TextureLoader.hpp"

// Window size definition
#define WINDOW_W 1280
#define WINDOW_H 720

// Asset directories definition, fallback to relative paths if not provided
#ifdef ASSET_DIR
#define MODELS_DIR ASSET_DIR "/models"
#define TEXTURES_DIR ASSET_DIR "/textures"
#else
#define MODELS_DIR "../../assets/models"
#define TEXTURES_DIR "../../assets/textures"
#endif
#ifndef SHADER_DIR
#define SHADER_DIR "../../shaders"
#endif

/* 
 * #TODO
 * do something about all this gl stuff (also apply anistrpoic filtering 16x )
 * probably in GLContext.cpp /.hpp      DONE
 * do the frame scheduler
 * need pipelineState.hpp probably for the frame scheduler and coordinator
 * turn the superloop into unity's Update()
 * optional: logger
 * rana should do react3dphysics    DONE
 * soliman should do textures and obj loading DONE
 * need to do scenes and scene manager DONE
 * decide what to do with these files:
 * system manager, system.hpp, component, asset manager,
 * renderSystem, TransformSystem <- we already have "TransformComponent"
 * 
 */


int main() {

    // ---------------------------
    // Window + GL Context Setup
    // ---------------------------
    // Initialize the window with specified width, height, and title.
    Window window(WINDOW_W, WINDOW_H, "Hello World");
    if (!window.get_handle()) return 1;  // If window creation failed, exit.

    // Initialize OpenGL context and enable default settings.
    if (!GLContext::init()) return 1;  // If GL context initialization fails, exit.
    GLContext::enable_default_render_settings();  // Enable default render settings.

    // ---------------------------
    // ImGui Initialization
    // ---------------------------
    IMGUI_CHECKVERSION();  // Check if ImGui version matches.
    ImGui::CreateContext();  // Create ImGui context.
    ImGuiIO& io = ImGui::GetIO();  // Get IO configuration.
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation for ImGui.

    ImGui::StyleColorsDark();  // Set ImGui style to dark mode.

    // Initialize ImGui backends for GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true);  // Initialize GLFW backend.
    ImGui_ImplOpenGL3_Init("#version 330");  // Initialize OpenGL3 backend.

    // ---------------------------
    // Load Shaders
    // ---------------------------
    ShaderManager shaderManager;

    // Load the shaders from files (vertex and fragment shaders)
    auto mainShader = shaderManager.loadShader("main",
        std::string(SHADER_DIR) + "/blackToWhite.vert",
        std::string(SHADER_DIR) + "/blackToWhite.frag"
    );
    auto houseShader = shaderManager.loadShader("textured",
        std::string(SHADER_DIR) + "/textured.vert",
        std::string(SHADER_DIR) + "/textured.frag"
    );
    auto houseMixedShader = shaderManager.loadShader("blended",
        std::string(SHADER_DIR) + "/blended.vert",
        std::string(SHADER_DIR) + "/blended.frag"
    );

    if (!mainShader) {
        fprintf(stderr, "Failed to load main shader\n");
        return 1;  // Exit if shader loading fails.
    }

    // ---------------------------
    // Create Materials
    // ---------------------------
    // Create some colored materials with different tint values.
    TintedMaterial blue, brown, green, yellow;

    blue.setShader(mainShader);
    blue.tint  = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);  // Blue tint

    brown.setShader(mainShader);
    brown.tint = glm::vec4(0.5f, 0.2f, 0.1f, 1.0f);  // Brown tint

    yellow.setShader(mainShader);
    yellow.tint = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);  // Yellow tint

    green.setShader(mainShader);
    green.tint = glm::vec4(0.4f, 1.0f, 0.2f, 1.0f);  // Green tint

    // ---------------------------
    // Create Textures
    // ---------------------------

    // Load textures from specified paths.
    std::string MoonTexturePath = std::string(TEXTURES_DIR) + "/moon.jpg";
    auto MoonTexture = TextureLoader::load(MoonTexturePath);
    std::cout << "Attempting to load texture: " << MoonTexturePath << std::endl;

    std::string HouseText = std::string(TEXTURES_DIR) + "/house/house.jpeg";
    Texture* HouseTexture = TextureLoader::load(HouseText);
    std::cout << "Attempting to load texture: " << HouseText << std::endl;

    std::string GlassText = std::string(TEXTURES_DIR) + "/house/glass.png";
    Texture* GlassTexture = TextureLoader::load(GlassText);
    std::cout << "Attempting to load texture: " << GlassText << std::endl;

    std::string SkyText = std::string(TEXTURES_DIR) + "/sky.jpg";
    Texture* SkyTexture = TextureLoader::load(SkyText);
    if (!SkyTexture) {
        std::cerr << "Warning: failed to load sky texture: " << SkyText << " (sky will be blank)\n";
    }

    // Material setup for house, blending textures.
    TexturedMaterial houseMaterial(houseShader, HouseTexture);
    TexturedMaterial houseMixedMaterial(houseMixedShader, HouseTexture);
    houseMixedMaterial.addTextureLayer(MoonTexture, BlendMode::Lerp, 0.4f);  // Add blended texture

    TexturedMaterial skyMaterial(houseShader, SkyTexture);
    skyMaterial.setShader(houseShader);

    // House glass material
    TexturedMaterial GlassMaterial(houseMixedShader, GlassTexture);

    // Load mesh from .obj file
    std::string meshPath = std::string(MODELS_DIR) + "/house/house.obj";
    std::cout << "Attempting to load mesh: " << meshPath << std::endl;
    Mesh* loadedMesh = MeshLoader::load(meshPath.c_str());

    // Create some primitive meshes (cube, glass plane, sky sphere)
    Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));
    Mesh glass_mesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));
    Mesh skySphere = Mesh::create_sphere({32, 16}, glm::vec3(0.0f), 1.0f, true);

    // Create MeshRenderers to upload and render these meshes
    MeshRenderer cube, house, glass, skyRenderer;

    // Upload mesh data to the GPU
    cube.upload(cubeMesh);
    house.upload(*loadedMesh);
    glass.upload(glass_mesh);
    skyRenderer.upload(skySphere);

    // ---------------------------
    // World + Camera Setup
    // ---------------------------
    // Setup the world and camera configuration
    World world;

    world.get_camera().position  = glm::vec3(10.f, 5.f, 10.f);
    world.get_camera().direction = glm::normalize(glm::vec3(-1.f, 0.f, -1.f));
    world.get_camera().up        = glm::vec3(0.f, 1.f, 0.f);
    world.get_camera().fov       = glm::radians(60.0f);  // Field of view
    world.get_camera().near      = 0.1f;  // Near clipping plane
    world.get_camera().far       = 100.0f;  // Far clipping plane

    // ---------------------------
    // Scene Graph (Entity Creation)
    // ---------------------------
    // Root entity for the scene
    Entity* root = world.createEntityWithParams(nullptr);

    // Create entities for water, island, sand, tree, house, windows, etc.
    Entity* water = world.createEntityWithParams(root, {0.f, 0.f, 0.f}, glm::quat(), {10.f, 1.f, 10.f}, &cube, &blue);

    // Create island as a parent entity for sand and tree entities
    Entity* island = world.createEntityWithParams(root);

    // Debugging: Print island rotation
    {
        glm::quat island_rotation = island->getRotation();
        std::cout << "Island rotation: " << island_rotation.x << ", " << island_rotation.y << ", " << island_rotation.z << ", " << island_rotation.w << std::endl;
    }

    // Sand on the island
    Entity* sand = world.createEntityWithParams(island, {0.f, 0.5f, 0.f}, glm::quat(), {2.f, 1.f, 2.f}, &cube, &yellow);

    // Test house entity with mixed textures
    Entity* testhouse = world.createEntityWithParams(root, {10.f, 1.f, 1.f}, glm::quat(), {1.f, 1.f, 1.f}, &house, &houseMixedMaterial);

    // Create windows as child entities of the house
    { // Window 1
        Entity* window1 = world.createEntityWithParams(
            testhouse,                              // parent entity
            { 2.0f, 1.5f, 0.0f },                  // local position relative to testhouse
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)), // rotation
            { 2.0f, 1.0f, 1.0f },                  // scale
            &glass, 
            &GlassMaterial
        );
    }

    { // Window 2
        Entity* window2 = world.createEntityWithParams(
            testhouse,
            { -0.2f, 2.0f, -2.75f },               // local position
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)), // rotation
            { 1.0f, 1.0f, 1.5f },                  // scale
            &glass,
            &GlassMaterial
        );
    }

    { // Window 3
        Entity* window3 = world.createEntityWithParams(
            testhouse,
            { -3.55f, 2.0f, 0.55f },               // local position
            glm::angleAxis(-glm::half_pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)), // rotation
            { 1.5f, 1.0f, 1.0f },                  // scale
            &glass,
            &GlassMaterial
        );
    }


    // Create tree base and trunk, and position leaves with offsets.
    Entity* tree = world.createEntityWithParams(island);
    Entity* tree_trunk = world.createEntityWithParams(tree, {0.f, 2.5f, 0.f}, glm::quat(), {0.5f, 5.f, 0.5f}, &cube, &brown);

    glm::vec3 leafOffsets[4] = {
        { 0.f, 0.f, 0.5f },
        { 0.f, 0.f, 0.75f },
        { 0.f, 0.f, 2.5f },
        { 0.f, 0.f, -2.5f }
    };

    // Create leaves as child entities of the tree with specific rotations and positions.
    for (int i = 0; i < 4; i++) {
        glm::quat rotY = glm::angleAxis(1.5f * i + 0.7f, glm::vec3(0.f, 1.f, 0.f));
        glm::quat rotZ = glm::angleAxis(0.7f, glm::vec3(0.f, 0.f, 1.f));

        world.createEntityWithParams(
            tree,
            {leafOffsets[i].x, 6.0f + leafOffsets[i].y, leafOffsets[i].z},
            rotY * rotZ,
            {0.5f, 2.f, 0.5f},
            &cube,
            &green
        );
    }

    // ---------------------------
    // Camera Controller Setup
    // ---------------------------
    // Initialize the camera controller to handle input and camera movement
    CameraController controller;
    controller.setup(window.get_handle(), &world.get_camera());

    // FPS tracking variables
    float last_time = static_cast<float>(glfwGetTime());
    float last_fps_time = last_time;
    int frameCount = 0;

    // ---------------------------
    // Main Loop
    // ---------------------------
    // Main loop for rendering and updating the scene.
    while (!glfwWindowShouldClose(window.get_handle())) {
        window.poll_events();

        float time = static_cast<float>(glfwGetTime());
        float delta_time = time - last_time;
        last_time = time;  // Update time for the next frame
        frameCount++;

        // Update FPS and print island's position and rotation every second
        if (time - last_fps_time >= 1.0f) {
            std::cout << "Island position: " << island->getPosition().x << ", " << island->getPosition().y << ", " << island->getPosition().z << std::endl;
            std::cout << "Island rotation: " << island->getRotation().x << ", " << island->getRotation().y << ", " << island->getRotation().z << ", " << island->getRotation().w << std::endl;
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;  // Reset frame count after printing FPS
            last_fps_time = time;
        }

        // Update the water scale and rotate the island
        water->setScale({10.f, 1.0f + 0.1f * glm::sin(time), 10.f});
        glm::quat delta_rot = glm::angleAxis(glm::radians(30.f) * delta_time, glm::vec3(0.f,1.f,0));
        island->setRotation(delta_rot * island->getRotation());

        // Update the controller and perform rendering.
        controller.update(delta_time);

        // Rendering setup: clear color and depth buffers
        int w, h;
        window.get_framebuffer_size(w, h);
        glViewport(0, 0, w, h);
        glClearColor(0.90f, 0.95f, 1.0f, 1.0f);  // Sky background color
        glClearDepth(1.0f);  // Clear depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear buffers

        // Get the view-projection matrix for the camera and pass it to the shaders
        int width, height;
        glfwGetFramebufferSize(window.get_handle(), &width, &height);

        glm::mat4 VP = world.get_camera().get_view_projection_matrix(glm::vec2(width, height));

        // ---------------------------
        // Draw sky (before other objects)
        // ---------------------------
        if (skySphere.get_vertex_count() > 0 && SkyTexture) {
            glm::mat4 M = glm::translate(glm::mat4(1.0f), world.get_camera().position)
                        * glm::scale(glm::mat4(1.0f), glm::vec3(500.0f));  // Scale sky sphere

            glm::mat4 MVP = VP * M;
            MVP[0][2] = MVP[0][3];  // Push sky sphere to the far plane
            MVP[1][2] = MVP[1][3];
            MVP[2][2] = MVP[2][3];
            MVP[3][2] = MVP[3][3];

            // Render sky sphere with the house shader
            glUseProgram(houseShader->getProgram());
            skyMaterial.setShader(houseShader);
            skyMaterial.setup();

            glUniform1i(houseShader->getUniformLocation("u_texture"), 0);  // Set texture unit 0
            glUniformMatrix4fv(houseShader->getUniformLocation("MVP"), 1, GL_FALSE, &MVP[0][0]);

            // Sky rendering settings
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);  // Sky sphere is wound inward; cull the outside
            glDepthFunc(GL_LEQUAL);  // Allow sky at far plane
            glDepthMask(GL_FALSE);  // Don't write depth buffer

            skyRenderer.draw();  // Render the sky sphere mesh

            // Restore render settings
            glDepthMask(GL_TRUE);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);
        }

        // ---------------------------
        // Render all entities in the world
        // ---------------------------
        for (Entity* root : world.getEntityManager().getRoots()) {
            world.getEntityManager().renderEntityRecursive(root, VP);
        }

        // --------------------------
        // ImGui New Frame
        // --------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --------------------------
        // Example UI Window
        // --------------------------
        ImGui::Begin("Hello ImGui");
        ImGui::Text("This is working!");

        ImGuiHelpers::ShowTransformInspector("Selected", testhouse);  // Show transform inspector for test house

        ImGui::End();

        // --------------------------
        // Render ImGui
        // --------------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers for the next frame
        window.swap_buffers();
    }

    // Cleanup resources at the end
    cube.destroy();
    house.destroy();
    glass.destroy();
    skyRenderer.destroy();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
