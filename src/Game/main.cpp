// GL and GLFW related headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

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
#define SKELETON_GLTF_PATH SOURCE_DIR "/assets/models/lowpoly_skeleton/scene.gltf"
#else
#define SWORDMAN_GLTF_PATH "tests/loading_files/the_swordman/scene.gltf"
#define SKELETON_GLTF_PATH "assets/models/lowpoly_skeleton/scene.gltf"
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
    Mesh plane_mesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));


    // Create MeshRenderers to upload and render these meshes
    MeshRenderer cube, house, glass, skyRenderer, ballRenderer, planeRenderer;

    // Upload mesh data to the GPU
    cube.upload(cubeMesh);
    if (loadedMesh) {
        house.upload(*loadedMesh);
    } else {
        std::cerr << "Warning: house.obj not found, using cube as fallback\n";
        house.upload(cubeMesh);
    }
    glass.upload(glass_mesh);
    skyRenderer.upload(skySphere);
    ballRenderer.upload(ballSphere);
    planeRenderer.upload(plane_mesh);

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
    
    player->setPosition({0.0f, 0.0f, 0.0f});
    player->attachCamera(&world.get_camera(), {0.0f, 2.0f, 4.0f}, {0.0f, 1.0f, 0.0f});
    player->setMoveSpeed(4.0f);
    player->setDodgeSpeed(10.0f);

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
    // Now we support multiple enemies
    struct EnemyData {
        std::unique_ptr<Enemy> enemy;
        HealthComponent health;
        HurtboxComponent hurtbox;
        Collider collider;
        Entity* skeletonEntity = nullptr;
        std::vector<SkinnedMeshRenderer*> skeletonRenderers;
        AnimationComponent* animComponent = nullptr;
        bool wasDead = false;
    };
    std::vector<EnemyData> enemies;
    
    // Skeleton model data (shared between all enemies)
    std::shared_ptr<ModelData> skeletonModelData;
    std::unique_ptr<SkinnedMaterial> skeletonMaterial;
    
    const float skeletonScale = 0.01f / 3.0f / 2.0f;
    const glm::quat skeletonRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Wave system variables (must be declared before spawnEnemy lambda)
    int currentWave = 1;
    int enemiesAlive = 1;
    int enemiesPerWave = 1;
    int enemyDamageMultiplier = 1;
    int totalEnemiesKilled = 0;
    bool waveInProgress = true;
    
    // Load skeleton model once
    if (skinnedShader) {
        skeletonMaterial = std::make_unique<SkinnedMaterial>(skinnedShader, nullptr);
        skeletonMaterial->set_animated(true);
        
        if (std::filesystem::exists(SKELETON_GLTF_PATH)) {
            std::cout << "Loading skeleton model: " << SKELETON_GLTF_PATH << std::endl;
            ModelData* rawModel = MeshLoader::load_gltf(SKELETON_GLTF_PATH);
            if (rawModel && rawModel->skeleton && !rawModel->meshes.empty()) {
                skeletonModelData = std::shared_ptr<ModelData>(rawModel);
                if (!skeletonModelData->textures.empty()) {
                    skeletonMaterial->setTexture(skeletonModelData->textures[0].get());
                }
                std::cout << "Skeleton has " << rawModel->animations.size() << " animations" << std::endl;
            } else {
                std::cerr << "Failed to load skeleton model.\n";
                delete rawModel;
            }
        }
    }
    
    // Function to spawn an enemy at a position
    auto spawnEnemy = [&](const glm::vec3& pos) {
        EnemyData data;
        data.enemy = CreateEnemy(world, root, &cube, &red);
        data.enemy->setPosition(pos);
        if (player) {
            data.enemy->setTarget(player->entity());
        }
        
        data.health.maxHP = 100;
        data.health.hp = 100;
        data.health.invulnDuration = 0.35f;
        data.health.set_spawn_point(pos);
        data.health.respawnDelay = 2.0f;
        
        data.hurtbox.halfExtents = {0.5f, 1.0f, 0.5f};
        data.hurtbox.localOffset = {0.0f, 1.0f, 0.0f};
        
        // Set up collider for enemy
        if (data.enemy->entity()) {
            data.collider.setParent(data.enemy->entity());
            data.collider.setHalfExtents(glm::vec3(0.4f, 1.0f, 0.4f));
            data.collider.setLocalOffset(glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        // Add skeleton model
        if (skeletonModelData && skeletonMaterial) {
            data.skeletonEntity = world.createEntityWithParams(
                data.enemy->entity(),
                glm::vec3(0.0f),
                skeletonRotation,
                {skeletonScale, skeletonScale, skeletonScale},
                nullptr, nullptr
            );
            data.skeletonEntity->setName("Skeleton");
            data.skeletonEntity->setModelData(skeletonModelData);
            
            for (auto& skinnedMesh : skeletonModelData->meshes) {
                SkinnedMeshRenderer* renderer = new SkinnedMeshRenderer();
                renderer->upload(skinnedMesh);
                data.skeletonRenderers.push_back(renderer);
            }
            data.skeletonEntity->setSkinnedRenderers(data.skeletonRenderers);
            data.skeletonEntity->setSkinnedMaterial(skeletonMaterial.get());
            
            data.animComponent = new AnimationComponent(skeletonModelData);
            data.skeletonEntity->addComponent(data.animComponent);
            if (data.animComponent->get_animation_count() > 0) {
                data.animComponent->play_animation(0, true);
            }
            
            // Hide red cube
            for (Entity* child : data.enemy->entity()->getChildren()) {
                if (child->getMesh()) {
                    child->setMesh(nullptr);
                    child->setMaterial(nullptr);
                }
            }
        }
        
        data.enemy->getCombat().damage = 15 * enemyDamageMultiplier;
        enemies.push_back(std::move(data));
    };
    
    // Spawn initial wave (1 enemy)
    spawnEnemy({-4.0f, 0.0f, 0.0f});

    // ---------------------------
    // XP Orb System
    // ---------------------------
    struct XPOrb {
        Entity* entity = nullptr;
        glm::vec3 velocity{0.0f};
        float lifetime = 10.0f;
        int xpValue = 25;
        bool collected = false;
    };
    std::vector<XPOrb> xpOrbs;
    int playerXP = 0;
    int playerLevel = 1;
    int xpToNextLevel = 100;
    bool showUpgradeMenu = false;  // Show upgrade menu on level up
    int pendingUpgrades = 0;  // Number of upgrades available
    
    // Player stats (upgradeable)
    float playerMoveSpeed = 4.0f;
    int playerDamage = 25;
    float playerDodgeSpeed = 10.0f;
    
    // Create a small cube mesh for XP orbs
    Mesh xpOrbMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(0.3f));
    MeshRenderer xpOrbRenderer;
    xpOrbRenderer.upload(xpOrbMesh);
    
    // Yellow/gold material for XP orbs
    TintedMaterial xpOrbMaterial;
    xpOrbMaterial.setShader(mainShader);
    xpOrbMaterial.tint = glm::vec4(1.0f, 0.85f, 0.0f, 1.0f);  // Gold color

    // Player health
    HealthComponent playerHealth;
    playerHealth.maxHP = 100;
    playerHealth.hp = playerHealth.maxHP;
    playerHealth.invulnDuration = 0.5f;
    playerHealth.respawnDelay = 3.0f;
    if (player) {
        playerHealth.set_spawn_point(player->getPosition());
    }

    HurtboxComponent playerHurtbox;
    playerHurtbox.halfExtents = {0.4f, 0.9f, 0.4f};
    playerHurtbox.localOffset = {0.0f, 1.0f, 0.0f};

    CombatComponent playerCombat;
    playerCombat.damage = 25;
    playerCombat.hitbox.halfExtents = {0.6f, 0.8f, 0.6f};
    playerCombat.hitbox.localOffset = {0.0f, 1.0f, 0.9f};


    // Create island as a parent entity for sand and tree entities
    Entity* island = world.createEntityWithParams(root);

    // Debugging: Print island rotation
    {
        glm::quat island_rotation = island->getRotation();
        std::cout << "Island rotation: " << island_rotation.x << ", " << island_rotation.y << ", " << island_rotation.z << ", " << island_rotation.w << std::endl;
    }


    Entity* asphalt = world.createEntityWithParams(root, {0.0f, 0.0f, 0.0f}, glm::quat(), {100.0f, 100.0f, 100.0f}, &planeRenderer, &AsphaltMaterial);
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
        glm::quat delta_rot = glm::angleAxis(glm::radians(30.f) * delta_time, glm::vec3(0.f,1.f,0));
        island->setRotation(delta_rot * island->getRotation());

        // Player input + update
        PlayerInput input;
        GLFWwindow* windowHandle = window.get_handle();
        
        // Only allow input if player is alive
        if (!playerHealth.dead) {
            if (glfwGetKey(windowHandle, GLFW_KEY_W) == GLFW_PRESS) input.move.y -= 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_S) == GLFW_PRESS) input.move.y += 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_D) == GLFW_PRESS) input.move.x += 1.0f;
            if (glfwGetKey(windowHandle, GLFW_KEY_A) == GLFW_PRESS) input.move.x -= 1.0f;
            input.block = glfwGetKey(windowHandle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            input.attack = glfwGetMouseButton(windowHandle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
                           || glfwGetKey(windowHandle, GLFW_KEY_J) == GLFW_PRESS;
            input.dodge = glfwGetKey(windowHandle, GLFW_KEY_SPACE) == GLFW_PRESS;
        }

        if (player) {
            player->setInput(input);
            player->update(delta_time);
        }

        // Combat: player vs all enemies
        bool attacking = player && player->isAttacking();
        playerHealth.update(delta_time);
        
        glm::vec3 playerPos = player ? player->getPosition() : glm::vec3(0.0f);
        glm::vec3 playerForward = glm::vec3(0.0f, 0.0f, 1.0f);
        if (player && player->entity()) {
            glm::mat4 rot = glm::mat4_cast(player->entity()->getRotation());
            playerForward = glm::normalize(glm::vec3(rot * glm::vec4(playerForward, 0.0f)));
        }
        
        // Setup player defense state
        DefenseState playerDefense;
        if (player) {
            playerDefense.blocking = player->isBlocking();
            playerDefense.dodging = player->isDodging();
            playerDefense.dodgeTimer = player->isDodging() ? 0.0f : 1.0f;
            playerDefense.dodgeWindow = 0.5f;
        }
        
        // Count alive enemies
        enemiesAlive = 0;
        
        // Update all enemies
        for (auto& enemyData : enemies) {
            if (!enemyData.enemy) continue;
            
            enemyData.health.update(delta_time);
            
            // Update AI if alive
            if (!enemyData.health.dead) {
                enemyData.enemy->update(delta_time);
                enemiesAlive++;
            }
            
            // Player attacks this enemy
            if (player && enemyData.enemy->entity()) {
                playerCombat.resolve_attack(
                    attacking,
                    playerPos,
                    playerForward,
                    enemyData.hurtbox,
                    enemyData.enemy->entity()->getPosition(),
                    enemyData.health
                );
                
                // Enemy attacks player
                if (!enemyData.health.dead && !playerHealth.dead) {
                    glm::vec3 enemyPos = enemyData.enemy->getPosition();
                    glm::vec3 enemyForward = glm::vec3(0.0f, 0.0f, 1.0f);
                    glm::mat4 enemyRot = glm::mat4_cast(enemyData.enemy->entity()->getRotation());
                    enemyForward = glm::normalize(glm::vec3(enemyRot * glm::vec4(enemyForward, 0.0f)));
                    
                    enemyData.enemy->getCombat().resolve_attack(
                        enemyData.enemy->isAttacking(),
                        enemyPos,
                        enemyForward,
                        playerHurtbox,
                        playerPos,
                        playerHealth,
                        &playerDefense
                    );
                }
            }
            
            // Handle enemy death
            if (enemyData.enemy->entity() && enemyData.health.dead) {
                enemyData.enemy->entity()->setScale({0.0f, 0.0f, 0.0f});
                if (enemyData.skeletonEntity) {
                    enemyData.skeletonEntity->setScale({0.0f, 0.0f, 0.0f});
                }
                
                // Spawn XP orbs (only once per death)
                if (!enemyData.wasDead) {
                    glm::vec3 deathPos = enemyData.enemy->getPosition();
                    int numOrbs = 3 + rand() % 3;
                    
                    for (int i = 0; i < numOrbs; i++) {
                        XPOrb orb;
                        orb.entity = world.createEntityWithParams(
                            root,
                            deathPos + glm::vec3(0.0f, 1.0f, 0.0f),
                            glm::quat(),
                            glm::vec3(0.3f),
                            &xpOrbRenderer,
                            &xpOrbMaterial
                        );
                        
                        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
                        float speed = 2.0f + (rand() % 100) / 50.0f;
                        orb.velocity = glm::vec3(
                            cos(angle) * speed * 0.5f,
                            4.0f + (rand() % 100) / 50.0f,
                            sin(angle) * speed * 0.5f
                        );
                        orb.xpValue = 10 + rand() % 15;
                        orb.lifetime = 15.0f;
                        xpOrbs.push_back(orb);
                    }
                    
                    totalEnemiesKilled++;
                    enemyData.wasDead = true;
                    std::cout << "Enemy killed! Total: " << totalEnemiesKilled << std::endl;
                }
            }
            
            // NOTE: Individual enemy respawn disabled - using wave system instead
        }
        
        // Wave progression - all enemies dead, spawn next wave
        if (enemiesAlive == 0 && waveInProgress) {
            currentWave++;
            enemiesPerWave++;
            
            // Every 5 waves, reset count but double damage
            if (enemiesPerWave > 5) {
                enemiesPerWave = 1;
                enemyDamageMultiplier *= 2;
            }
            
            std::cout << "WAVE " << currentWave << " START! Spawning " << enemiesPerWave << " enemies. DMG x" << enemyDamageMultiplier << std::endl;
            
            // Clean up old enemy resources before clearing
            // Note: We hide entities but don't delete them to prevent crashes
            for (auto& enemyData : enemies) {
                // Hide skeleton and clear its renderer references
                if (enemyData.skeletonEntity) {
                    enemyData.skeletonEntity->setSkinnedRenderers({});
                    enemyData.skeletonEntity->setSkinnedMaterial(nullptr);
                    enemyData.skeletonEntity->setScale({0.0f, 0.0f, 0.0f});
                }
                // Hide enemy entity
                if (enemyData.enemy && enemyData.enemy->entity()) {
                    enemyData.enemy->entity()->setScale({0.0f, 0.0f, 0.0f});
                }
                // Destroy GPU resources but keep renderer objects alive
                // (they'll be leaked but prevents crash)
                for (SkinnedMeshRenderer* renderer : enemyData.skeletonRenderers) {
                    if (renderer) {
                        renderer->destroy();  // Free GPU resources
                    }
                }
                // Don't delete renderers - just clear our tracking
                enemyData.skeletonRenderers.clear();
                enemyData.animComponent = nullptr;
                enemyData.skeletonEntity = nullptr;
            }
            
            // Clear old enemies and spawn new wave
            enemies.clear();
            for (int i = 0; i < enemiesPerWave; i++) {
                float angle = (float)i / (float)enemiesPerWave * 6.28318f;
                float radius = 5.0f + (float)(rand() % 30) / 10.0f;
                glm::vec3 spawnPos = playerPos + glm::vec3(cos(angle) * radius, 1.0f, sin(angle) * radius);
                spawnEnemy(spawnPos);
            }
            enemiesAlive = enemiesPerWave;
        }

        // Player respawn
        static bool playerWasDead = false;
        if (player && playerHealth.dead) {
            if (!playerWasDead) {
                playerWasDead = true;
            }
        }
        if (player && playerHealth.ready_to_respawn()) {
            playerHealth.respawn();
            player->setPosition(playerHealth.spawnPos);
            playerWasDead = false;
        }

        // ---------------------------
        // Update XP Orbs
        // ---------------------------
        // playerPos already declared above
        const float pickupRadius = 1.5f;
        const float gravity = 9.8f;
        
        for (auto& orb : xpOrbs) {
            if (!orb.entity || orb.collected) continue;
            
            // Apply gravity
            orb.velocity.y -= gravity * delta_time;
            
            // Update position
            glm::vec3 orbPos = orb.entity->getPosition();
            orbPos += orb.velocity * delta_time;
            
            // Ground collision (y = 0.5 for orb center)
            if (orbPos.y < 0.5f) {
                orbPos.y = 0.5f;
                orb.velocity.y = 0.0f;
                orb.velocity.x *= 0.8f;  // Friction
                orb.velocity.z *= 0.8f;
            }
            
            orb.entity->setPosition(orbPos);
            
            // Rotate the orb for visual effect
            glm::quat spin = glm::angleAxis(delta_time * 3.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            orb.entity->setRotation(spin * orb.entity->getRotation());
            
            // Check pickup collision with player
            if (player) {
                float dist = glm::length(orbPos - playerPos);
                if (dist < pickupRadius) {
                    // Collect the orb
                    playerXP += orb.xpValue;
                    orb.collected = true;
                    orb.entity->setScale(glm::vec3(0.0f));  // Hide it
                    
                    // Level up check
                    while (playerXP >= xpToNextLevel) {
                        playerXP -= xpToNextLevel;
                        playerLevel++;
                        xpToNextLevel = 100 * playerLevel;  // Increase XP needed per level
                        pendingUpgrades++;  // Add an upgrade point
                        showUpgradeMenu = true;  // Show upgrade menu
                        std::cout << "LEVEL UP! Now level " << playerLevel << std::endl;
                    }
                }
            }
            
            // Lifetime
            orb.lifetime -= delta_time;
            if (orb.lifetime <= 0.0f) {
                orb.collected = true;
                orb.entity->setScale(glm::vec3(0.0f));
            }
        }
        
        // Clean up collected orbs
        xpOrbs.erase(
            std::remove_if(xpOrbs.begin(), xpOrbs.end(), 
                [](const XPOrb& o) { return o.collected; }),
            xpOrbs.end()
        );

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
        
        // Player-Enemy collision
        for (auto& enemyData : enemies) {
            if (player && enemyData.enemy && enemyData.enemy->entity() && !enemyData.health.dead) {
                PhysicsCollisionSystem::resolveStaticCollision(
                    player->entity(),
                    playerCollider,
                    enemyData.collider
                );
            }
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

        // Draw health bars for all enemies
        for (auto& enemyData : enemies) {
            if (enemyData.enemy && enemyData.enemy->entity() && !enemyData.health.dead) {
                HealthBarSystem::draw_over_entity(
                    enemyData.health,
                    enemyData.hurtbox,
                    *enemyData.enemy->entity(),
                    VP,
                    width,
                    height
                );
            }
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
        ImGui::Text("Enemies: %d alive", enemiesAlive);
        for (size_t i = 0; i < enemies.size(); i++) {
            float hpFrac = enemies[i].health.maxHP > 0
                ? static_cast<float>(enemies[i].health.hp) / static_cast<float>(enemies[i].health.maxHP)
                : 0.0f;
            hpFrac = glm::clamp(hpFrac, 0.0f, 1.0f);
            char label[32];
            snprintf(label, sizeof(label), "Enemy %zu", i + 1);
            ImGui::Text("%s", label);
            ImGui::ProgressBar(hpFrac, ImVec2(0.0f, 0.0f));
        }

        ImGui::End();
        
        // --------------------------
        // Player Health Bar (top-right corner)
        // --------------------------
        {
            ImGuiIO& io = ImGui::GetIO();
            float barWidth = 200.0f;
            float barHeight = 25.0f;
            float padding = 20.0f;
            
            ImVec2 barPos(io.DisplaySize.x - barWidth - padding, padding);
            ImVec2 barEnd(barPos.x + barWidth, barPos.y + barHeight);
            
            float playerHpFrac = playerHealth.maxHP > 0
                ? static_cast<float>(playerHealth.hp) / static_cast<float>(playerHealth.maxHP)
                : 0.0f;
            playerHpFrac = glm::clamp(playerHpFrac, 0.0f, 1.0f);
            
            ImDrawList* drawList = ImGui::GetForegroundDrawList();
            
            // Background
            drawList->AddRectFilled(barPos, barEnd, IM_COL32(40, 40, 40, 200), 4.0f);
            
            // Health fill
            ImVec2 fillEnd(barPos.x + barWidth * playerHpFrac, barEnd.y);
            ImU32 healthColor = playerHpFrac > 0.5f ? IM_COL32(80, 200, 80, 220) :
                               playerHpFrac > 0.25f ? IM_COL32(220, 180, 50, 220) :
                               IM_COL32(220, 60, 60, 220);
            drawList->AddRectFilled(barPos, fillEnd, healthColor, 4.0f);
            
            // Border
            drawList->AddRect(barPos, barEnd, IM_COL32(255, 255, 255, 180), 4.0f, 0, 2.0f);
            
            // Text
            char hpText[32];
            snprintf(hpText, sizeof(hpText), "HP: %d / %d", playerHealth.hp, playerHealth.maxHP);
            ImVec2 textSize = ImGui::CalcTextSize(hpText);
            ImVec2 textPos(barPos.x + (barWidth - textSize.x) * 0.5f, barPos.y + (barHeight - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), hpText);
            
            if (playerHealth.dead) {
                const char* deadText = "RESPAWNING...";
                ImVec2 deadSize = ImGui::CalcTextSize(deadText);
                ImVec2 deadPos(barPos.x + (barWidth - deadSize.x) * 0.5f, barEnd.y + 5.0f);
                drawList->AddText(deadPos, IM_COL32(255, 100, 100, 255), deadText);
            }
        }
        
        // --------------------------
        // XP Bar (below health bar)
        // --------------------------
        {
            ImGuiIO& io = ImGui::GetIO();
            float barWidth = 200.0f;
            float barHeight = 20.0f;
            float padding = 20.0f;
            float yOffset = 55.0f;  // Below health bar
            
            ImVec2 barPos(io.DisplaySize.x - barWidth - padding, padding + yOffset);
            ImVec2 barEnd(barPos.x + barWidth, barPos.y + barHeight);
            
            float xpFrac = xpToNextLevel > 0
                ? static_cast<float>(playerXP) / static_cast<float>(xpToNextLevel)
                : 0.0f;
            xpFrac = glm::clamp(xpFrac, 0.0f, 1.0f);
            
            ImDrawList* drawList = ImGui::GetForegroundDrawList();
            
            // Background
            drawList->AddRectFilled(barPos, barEnd, IM_COL32(40, 40, 40, 200), 4.0f);
            
            // XP fill (gold/yellow color)
            ImVec2 fillEnd(barPos.x + barWidth * xpFrac, barEnd.y);
            drawList->AddRectFilled(barPos, fillEnd, IM_COL32(255, 215, 0, 220), 4.0f);
            
            // Border
            drawList->AddRect(barPos, barEnd, IM_COL32(255, 255, 255, 180), 4.0f, 0, 2.0f);
            
            // Text
            char xpText[48];
            snprintf(xpText, sizeof(xpText), "LVL %d | XP: %d / %d", playerLevel, playerXP, xpToNextLevel);
            ImVec2 textSize = ImGui::CalcTextSize(xpText);
            ImVec2 textPos(barPos.x + (barWidth - textSize.x) * 0.5f, barPos.y + (barHeight - textSize.y) * 0.5f);
            drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), xpText);
            
            // Wave info below XP bar
            char waveText[64];
            snprintf(waveText, sizeof(waveText), "Wave %d | Enemies: %d/%d | DMG: x%d", currentWave, enemiesAlive, enemiesPerWave, enemyDamageMultiplier);
            ImVec2 waveSize = ImGui::CalcTextSize(waveText);
            ImVec2 wavePos(barPos.x + (barWidth - waveSize.x) * 0.5f, barEnd.y + 5.0f);
            drawList->AddText(wavePos, IM_COL32(200, 200, 200, 255), waveText);
        }
        
        // --------------------------
        // Upgrade Menu (on level up)
        // --------------------------
        if (showUpgradeMenu && pendingUpgrades > 0) {
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 windowSize(300, 200);
            ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);
            
            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
            ImGui::Begin("LEVEL UP!", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            
            ImGui::Text("Choose an upgrade! (%d available)", pendingUpgrades);
            ImGui::Separator();
            
            char speedText[64];
            snprintf(speedText, sizeof(speedText), "Speed (%.1f -> %.1f)", playerMoveSpeed, playerMoveSpeed + 0.5f);
            if (ImGui::Button(speedText, ImVec2(280, 40))) {
                playerMoveSpeed += 0.5f;
                if (player) player->setMoveSpeed(playerMoveSpeed);
                pendingUpgrades--;
                if (pendingUpgrades <= 0) showUpgradeMenu = false;
            }
            
            char damageText[64];
            snprintf(damageText, sizeof(damageText), "Damage (%d -> %d)", playerDamage, playerDamage + 5);
            if (ImGui::Button(damageText, ImVec2(280, 40))) {
                playerDamage += 5;
                playerCombat.damage = playerDamage;
                pendingUpgrades--;
                if (pendingUpgrades <= 0) showUpgradeMenu = false;
            }
            
            char dodgeText[64];
            snprintf(dodgeText, sizeof(dodgeText), "Agility (%.1f -> %.1f)", playerDodgeSpeed, playerDodgeSpeed + 1.0f);
            if (ImGui::Button(dodgeText, ImVec2(280, 40))) {
                playerDodgeSpeed += 1.0f;
                if (player) player->setDodgeSpeed(playerDodgeSpeed);
                pendingUpgrades--;
                if (pendingUpgrades <= 0) showUpgradeMenu = false;
            }
            
            ImGui::End();
        }

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
    // Cleanup enemy skeleton renderers
    for (auto& enemyData : enemies) {
        for (SkinnedMeshRenderer* renderer : enemyData.skeletonRenderers) {
            if (renderer) {
                renderer->destroy();
                delete renderer;
            }
        }
    }
    delete swordmanAnim;
    cube.destroy();
    house.destroy();
    glass.destroy();
    skyRenderer.destroy();
    ballRenderer.destroy();
    xpOrbRenderer.destroy();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
