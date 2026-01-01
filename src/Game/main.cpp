// GL and GLFW related headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

// ImGui related headers (for GUI)
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

// Custom headers for engine-related functionality
#include "../engine/platform/Window.hpp"
#include "../engine/gl/GLContext.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/ecs/ShaderManager.hpp"
#include "../engine/assets/MaterialManager.hpp"
#include "../engine/ecs/AnimationComponent.hpp"
#include "../engine/components/CombatComponent.hpp"
#include "../engine/components/HealthComponent.hpp"
#include "../engine/components/HitboxComponent.hpp"
#include "../engine/components/HurtboxComponent.hpp"
#include "../engine/components/MeshRenderer.hpp"
#include "../engine/components/SkinnedMeshRenderer.hpp"
#include "../engine/gl/Mesh.hpp"
#include "../engine/ecs/Collider.hpp"
#include "../engine/utils/Im_GUI_Inspector.hpp"
#include "../engine/assets/MeshLoader.hpp"
#include "../engine/assets/SkinnedMaterial.hpp"
#include "../engine/assets/TexturedMaterial.hpp"
#include "../engine/assets/TextureLoader.hpp"
#include "../engine/assets/LitMaterial.hpp"
#include "../engine/systems/PhysicsCollisionSystem.hpp"
#include "../engine/systems/HealthBarSystem.hpp"
#include "../engine/systems/LightSystem.hpp"
#include "Entities/Player.hpp"
#include "Entities/Crusader.hpp"
#include "Entities/Enemy.hpp"

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
#ifdef SOURCE_DIR
#define SWORDMAN_GLTF_PATH SOURCE_DIR "/tests/loading_files/the_swordman/scene.gltf"
#else
#define SWORDMAN_GLTF_PATH "tests/loading_files/the_swordman/scene.gltf"
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

    auto lightShader = shaderManager.loadShader("light",
        std::string(SHADER_DIR) + "/light.vert",
        std::string(SHADER_DIR) + "/light.frag"
    );

    auto skinnedShader = shaderManager.loadShader("skinned",
        std::string(SHADER_DIR) + "/skinned.vert",
        std::string(SHADER_DIR) + "/skinned.frag"
    );

    if (!mainShader) {
        fprintf(stderr, "Failed to load main shader\n");
        return 1;  // Exit if shader loading fails.
    }

    if (!houseShader) {
        fprintf(stderr, "Failed to load house shader\n");
        return 1;  // Exit if shader loading fails.
    }

    if (!houseMixedShader) {
        fprintf(stderr, "Failed to load houseMixed shader\n");
        return 1;  // Exit if shader loading fails.
    }

    if (!skinnedShader) {
        fprintf(stderr, "Warning: failed to load skinned shader (swordman will not render)\n");
    }


    // ---------------------------
    // Create Materials
    // ---------------------------
    // Create some colored materials with different tint values.
    TintedMaterial blue, brown, green, yellow, red;

    blue.setShader(mainShader);
    blue.tint  = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);  // Blue tint

    brown.setShader(mainShader);
    brown.tint = glm::vec4(0.5f, 0.2f, 0.1f, 1.0f);  // Brown tint

    yellow.setShader(mainShader);
    yellow.tint = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);  // Yellow tint

    green.setShader(mainShader);
    green.tint = glm::vec4(0.4f, 1.0f, 0.2f, 1.0f);  // Green tint

    red.setShader(mainShader);
    red.tint = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);  // Red tint

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

    std::string AsphaltSpecularPath = std::string(TEXTURES_DIR) + "/asphalt/specular.jpg";
    auto AsphaltSpecular = TextureLoader::load(AsphaltSpecularPath);
    std::cout << "Attempting to load texture: " << AsphaltSpecularPath << std::endl;

    std::string AsphaltRoughnessPath = std::string(TEXTURES_DIR) + "/asphalt/roughness.jpg";
    auto AsphaltRoughness = TextureLoader::load(AsphaltRoughnessPath);
    std::cout << "Attempting to load texture: " << AsphaltRoughnessPath << std::endl;

    std::string AsphaltEmissivePath = std::string(TEXTURES_DIR) + "/asphalt/emissive.jpg";
    auto AsphaltEmissive = TextureLoader::load(AsphaltEmissivePath);
    std::cout << "Attempting to load texture: " << AsphaltEmissivePath << std::endl;

    std::string AsphaltAlbedoPath = std::string(TEXTURES_DIR) + "/asphalt/albedo.jpg";
    auto AsphaltAlbedo = TextureLoader::load(AsphaltAlbedoPath);
    std::cout << "Attempting to load texture: " << AsphaltAlbedoPath << std::endl;

    std::string SuzanneAOPath = std::string(TEXTURES_DIR) + "/suzanne/ambient_occlusion.jpg";
    auto SuzanneAO = TextureLoader::load(SuzanneAOPath);
    std::cout << "Attempting to load texture: " << SuzanneAOPath << std::endl;

    // Material setup for house, blending textures.
    TexturedMaterial houseMaterial(houseShader, HouseTexture);

    LitMaterial AsphaltMaterial(lightShader,
                                AsphaltAlbedo,
                                AsphaltSpecular,
                                AsphaltRoughness,
                                AsphaltEmissive,
                                SuzanneAO
                                );


    TexturedMaterial houseMixedMaterial(houseMixedShader, HouseTexture);
    houseMixedMaterial.addTextureLayer(MoonTexture, BlendMode::Lerp, 0.4f);  // Add blended texture

    TexturedMaterial skyMaterial(houseShader, SkyTexture);
    skyMaterial.setShader(houseShader);

    // House glass material
    TexturedMaterial GlassMaterial(houseMixedShader, GlassTexture);
    
    // 
    // Create Lighting stuff
    //
    LightSystem lightManager;
    

    lightManager.addLight(Light(LightType::DIRECTIONAL, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    lightManager.addLight(Light(LightType::POINT, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 5.0f, 0.0f)));
    lightManager.addLight(Light(LightType::SPOT, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(2.0f, 4.0f, 0.0f), glm::vec3(-1.0f, -1.0f, 0.0f), glm::radians(30.0f), glm::radians(60.0f)));

    // Set up the lights in the shader
    // houseMixedMaterial.setup();
    AsphaltMaterial.setup();
    // lightShader->use();
    // houseMixedShader->use();
    // houseShader->use();
    // mainShader->use();

    // Load imported meshes (obj)
    // Load mesh from .obj file
    std::string meshPath = std::string(MODELS_DIR) + "/house/house.obj";
    std::cout << "Attempting to load mesh: " << meshPath << std::endl;
    Mesh* loadedMesh = MeshLoader::load(meshPath.c_str());

    // Create some primitive meshes (cube, glass plane, sky sphere)
    Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));
    Mesh glass_mesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));
    Mesh skySphere = Mesh::create_sphere({32, 16}, glm::vec3(0.0f), 1.0f, true);
    Mesh ballSphere = Mesh::create_sphere();

    // Mesh asphaltMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));

    // Create MeshRenderers to upload and render these meshes
    MeshRenderer cube, house, glass, skyRenderer, ballRenderer;

    // Upload mesh data to the GPU
    cube.upload(cubeMesh);
    house.upload(*loadedMesh);
    glass.upload(glass_mesh);
    skyRenderer.upload(skySphere);
    ballRenderer.upload(ballSphere);

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


    // compute initial view-projection and set light shader globals (use proper viewport)
    int width1, height1;
    glfwGetFramebufferSize(window.get_handle(), &width1, &height1);
        
    // glm::mat4 VP1 = world.get_camera().get_view_projection_matrix(glm::vec2(width1, height1));
    // lightShader->use();
    // lightShader->setUniform("viewProj", VP1);
    // glm::mat4 model = asphalt.getLocalMatrix();
    // shader->setUniform("model", model);
    // shader->setUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));


    // lightShader->setUniform("normalMatrix", normalMatrix);
    lightShader->setUniform("camera_pos", world.get_camera().position);
    lightShader->setUniform("ambient", glm::vec3(0.05f));

    lightManager.setupLightsInShader(lightShader);
    
    // ---------------------------
    // Scene Graph (Entity Creation)
    // ---------------------------
    // Root entity for the scene
    Entity* root = world.createEntityWithParams(nullptr);

    std::shared_ptr<ModelData> swordmanModelData;
    std::vector<SkinnedMeshRenderer*> swordmanRenderers;
    std::unique_ptr<SkinnedMaterial> swordmanMaterial;
    AnimationComponent* swordmanAnim = nullptr;
    Entity* swordmanEntity = nullptr;

    // ---------------------------
    // Player + Enemy Setup
    // ---------------------------
    
    // std::unique_ptr<Player> holding a Crusader
    // second cube and brown are the weapon mesh and material
    auto player = std::make_unique<Crusader>(world, root, &cube, &green, &cube, &brown);

    //auto makes us not need to explicitly declare player as unique_ptr
    // std::unique_ptr<Player> player = std::make_unique<Crusader>(world, root, &cube, &green, &cube, &brown);
    
    player->setPosition({0.0f, 1.0f, 0.0f});
    player->attachCamera(&world.get_camera(), {0.0f, 2.0f, 4.0f}, {0.0f, 1.0f, 0.0f});

    const float swordmanScale = 0.01f;
    const glm::vec3 swordmanLocalOffset{0.0f, 0.9f, 0.0f};
    if (skinnedShader) {
        swordmanMaterial = std::make_unique<SkinnedMaterial>(skinnedShader, nullptr);
        swordmanMaterial->set_animated(true);
    }

    if (player && swordmanMaterial) {
        if (std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
            std::cout << "Loading swordman model: " << SWORDMAN_GLTF_PATH << std::endl;
            ModelData* rawModel = MeshLoader::load_gltf(SWORDMAN_GLTF_PATH);
            if (rawModel && rawModel->skeleton && !rawModel->meshes.empty()) {
                swordmanModelData = std::shared_ptr<ModelData>(rawModel);
                swordmanEntity = world.createEntityWithParams(
                    player->entity(),
                    swordmanLocalOffset,
                    glm::quat(),
                    {swordmanScale, swordmanScale, swordmanScale},
                    nullptr,
                    nullptr
                );
                swordmanEntity->setName("Swordman");
                swordmanEntity->setModelData(swordmanModelData);

                for (auto& skinnedMesh : swordmanModelData->meshes) {
                    SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
                    renderer->upload(skinnedMesh);
                    swordmanRenderers.push_back(renderer);
                }
                swordmanEntity->setSkinnedRenderers(swordmanRenderers);
                swordmanEntity->setSkinnedMaterial(swordmanMaterial.get());

                swordmanAnim = new AnimationComponent(swordmanModelData);
                swordmanEntity->addComponent(swordmanAnim);
                if (swordmanAnim->get_animation_count() > 0) {
                    swordmanAnim->play_animation(0, true);
                } else {
                    std::cerr << "Swordman model loaded but has no animations.\n";
                }

                if (!swordmanModelData->textures.empty()) {
                    swordmanMaterial->setTexture(swordmanModelData->textures[0].get());
                }

                if (player->getBody()) {
                    player->getBody()->setMesh(nullptr);
                    player->getBody()->setMaterial(nullptr);
                }
                if (player->getWeapon()) {
                    player->getWeapon()->setMesh(nullptr);
                    player->getWeapon()->setMaterial(nullptr);
                }
            } else {
                std::cerr << "Failed to load swordman model or missing skeleton/meshes.\n";
                delete rawModel;
            }
        } else {
            std::cerr << "Swordman model not found: " << SWORDMAN_GLTF_PATH << std::endl;
        }
    }

    // is a unique ptr cuz we have many enemies that are unrelated
    std::unique_ptr<Enemy> enemy = CreateEnemy(world, root, &cube, &red);
    enemy->setPosition({-4.0f, 1.0f, 0.0f});

    HealthComponent enemyHealth;
    enemyHealth.maxHP = 100;
    enemyHealth.hp = enemyHealth.maxHP;
    enemyHealth.invulnDuration = 0.35f;
    enemyHealth.set_spawn_point(enemy->entity()->getPosition());
    enemyHealth.respawnDelay = 2.0f;

    HurtboxComponent enemyHurtbox;
    enemyHurtbox.halfExtents = {0.5f, 1.0f, 0.5f};
    enemyHurtbox.localOffset = {0.0f, 1.0f, 0.0f};

    CombatComponent playerCombat;
    playerCombat.damage = 25;
    playerCombat.hitbox.halfExtents = {0.6f, 0.8f, 0.6f};
    playerCombat.hitbox.localOffset = {0.0f, 1.0f, 0.9f};

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

    Entity* asphalt = world.createEntityWithParams(island, {4.0f, 4.0f, 4.0f}, glm::quat(), {1.0f, 1.0f, 1.0f}, &ballRenderer, &AsphaltMaterial);
    // Test house entity with mixed textures
    Entity* testhouse = world.createEntityWithParams(root, {10.f, 1.f, 1.f}, glm::quat(), {1.f, 1.f, 1.f}, &house, &houseMixedMaterial);
    Entity* collisionSphere = world.createEntityWithParams(root, {4.f, 2.0f, -3.f}, glm::quat(), {0.6f, 0.6f, 0.6f}, &ballRenderer, &blue);
    Collider playerCollider;
    Collider houseCollider;
    Collider sphereCollider;
    bool houseCollisionReady = false;
    bool sphereCollisionReady = false;
    const float sphereMoveSpeed = 3.0f;

    if (player) {
        playerCollider.setParent(player->getBody());
        playerCollider.setHalfExtents(glm::vec3(0.3f, 0.45f, 0.3f));
    }

    if (loadedMesh) {
        PhysicsCollisionSystem::MeshBounds houseBounds;
        if (PhysicsCollisionSystem::computeMeshBounds(*loadedMesh, houseBounds)) {
            houseCollider.setParent(testhouse);
            houseCollider.setLocalOffset(houseBounds.center);
            houseCollider.setHalfExtents(houseBounds.halfExtents);
            houseCollisionReady = true;
        }
    }
    if (collisionSphere) {
        PhysicsCollisionSystem::MeshBounds sphereBounds;
        if (PhysicsCollisionSystem::computeMeshBounds(ballSphere, sphereBounds)) {
            sphereCollider.setParent(collisionSphere);
            sphereCollider.setLocalOffset(sphereBounds.center);
            sphereCollider.setHalfExtents(sphereBounds.halfExtents);
            sphereCollisionReady = true;
        }
    }

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
        { 0.f, 0.75f, 0.75f },
        { 0.f, 0.75f, 2.5f },
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

        // Player input + update
        PlayerInput input;
        GLFWwindow* windowHandle = window.get_handle();
        if (glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS) input.move.y -= 1.0f;
        if (glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS) input.move.y += 1.0f;
        if (glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS) input.move.x += 1.0f;
        if (glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS) input.move.x -= 1.0f;
        input.block = glfwGetKey(windowHandle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        input.attack = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
                       || glfwGetKey(windowHandle, GLFW_KEY_J) == GLFW_PRESS;
        input.dodge = glfwGetKey(windowHandle, GLFW_KEY_SPACE) == GLFW_PRESS;

        if (player) {
            player->setInput(input);
            player->update(delta_time);
        }

        // Combat: player hitbox vs enemy hurtbox
        bool attacking = player && player->isAttacking();
        enemyHealth.update(delta_time);

        if (player && enemy && enemy->entity()) {
            glm::vec3 playerPos = player->getPosition();
            glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
            if (player->entity()) {
                glm::mat4 rot = glm::mat4_cast(player->entity()->getRotation());
                forward = glm::normalize(glm::vec3(rot * glm::vec4(forward, 0.0f)));
            }

            playerCombat.resolve_attack(
                attacking,
                playerPos,
                forward,
                enemyHurtbox,
                enemy->entity()->getPosition(),
                enemyHealth
            );
        }

        if (enemy && enemy->entity() && enemyHealth.dead) {
            enemy->entity()->setScale({0.0f, 0.0f, 0.0f});
        }

        if (enemy && enemy->entity() && enemyHealth.ready_to_respawn()) {
            enemyHealth.respawn();
            enemy->setPosition(enemyHealth.spawnPos);
            enemy->entity()->setScale({1.0f, 1.0f, 1.0f});
        }

        if (collisionSphere) {
            glm::vec3 sphereMove(0.0f);
            if (glfwGetKey(windowHandle, GLFW_KEY_UP) == GLFW_PRESS) sphereMove.z -= 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_DOWN) == GLFW_PRESS) sphereMove.z += 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_LEFT) == GLFW_PRESS) sphereMove.x -= 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_RIGHT) == GLFW_PRESS) sphereMove.x += 1.0f;

            float moveLen = glm::length(sphereMove);
            if (moveLen > 0.001f) {
                sphereMove /= moveLen;
                collisionSphere->setPosition(
                    collisionSphere->getPosition() + sphereMove * sphereMoveSpeed * delta_time
                );
            }
        }

        if (player && houseCollisionReady) {
            PhysicsCollisionSystem::resolveStaticCollision(
                player->entity(),
                playerCollider,
                houseCollider
            );
        }
        if (collisionSphere && houseCollisionReady && sphereCollisionReady) {
            PhysicsCollisionSystem::resolveStaticCollision(
                collisionSphere,
                sphereCollider,
                houseCollider
            );
        }
        if (player && sphereCollisionReady) {
            PhysicsCollisionSystem::resolveStaticCollision(
                player->entity(),
                playerCollider,
                sphereCollider
            );
        }

        for (Entity* rootEntity : world.getEntityManager().getRoots()) {
            if (rootEntity->getParent()) {
                continue;
            }
            rootEntity->updateComponents(delta_time);
        }

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
        for (Entity* rootEntity : world.getEntityManager().getRoots()) {
            if (rootEntity->getParent()) {
                continue;
            }
            world.getEntityManager().renderEntityRecursive(rootEntity, VP);
        }

        // --------------------------
        // ImGui New Frame
        // --------------------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (enemy && enemy->entity()) {
            HealthBarSystem::draw_over_entity(
                enemyHealth,
                enemyHurtbox,
                *enemy->entity(),
                VP,
                width,
                height
            );
        }

        // --------------------------
        // Example UI Window
        // --------------------------
        ImGui::Begin("Hello ImGui");
        ImGui::Text("This is working!");

        ImGuiHelpers::ShowTransformInspector("Selected", testhouse);  // Show transform inspector for test house
        if (player) {
            glm::vec3 pos = player->getPosition();
            ImGui::Separator();
            ImGui::Text("Player pos: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
            ImGui::Text("Block: %s", player->isBlocking() ? "yes" : "no");
            ImGui::Text("Attack: %s", player->isAttacking() ? "yes" : "no");
            ImGui::Text("Dodge: %s", player->isDodging() ? "yes" : "no");
        }
        ImGui::Separator();
        float hpFrac = enemyHealth.maxHP > 0
            ? static_cast<float>(enemyHealth.hp) / static_cast<float>(enemyHealth.maxHP)
            : 0.0f;
        hpFrac = glm::clamp(hpFrac, 0.0f, 1.0f);
        ImGui::Text("Enemy HP");
        ImGui::ProgressBar(hpFrac, ImVec2(0.0f, 0.0f));
        if (enemyHealth.dead) {
            ImGui::Text("Enemy respawning...");
        }

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
    for (SkinnedMeshRenderer* renderer : swordmanRenderers) {
        if (renderer) {
            renderer->destroy();
            delete renderer;
        }
    }
    delete swordmanAnim;
    cube.destroy();
    house.destroy();
    glass.destroy();
    skyRenderer.destroy();
    ballRenderer.destroy();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
