#include "GameApplication.hpp"

#include "GameAnimationFactory.hpp"
#include "GameEntityFactory.hpp"
#include "GameSystems.hpp"
#include "GameWeaponFactory.hpp"
#include "Entities/Crusader.hpp"
#include "Entities/Enemy.hpp"

#include "../engine/components/CombatComponent.hpp"
#include "../engine/components/HealthComponent.hpp"
#include "../engine/components/HurtboxComponent.hpp"
#include "../engine/components/Light.hpp"
#include "../engine/gl/GLContext.hpp"
#include "../engine/gl/Mesh.hpp"
#include "../engine/platform/Window.hpp"
#include "../engine/systems/HealthBarSystem.hpp"
#include "../engine/systems/PhysicsCollisionSystem.hpp"
#include "../engine/systems/TransformSystem.hpp"
#include "../engine/utils/Im_GUI_Inspector.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>

namespace {
constexpr int WINDOW_W = 1280;
constexpr int WINDOW_H = 720;
constexpr float CHARACTER_LIGHT_HEIGHT = 0.35f;
constexpr float PLAYER_UNDER_LIGHT_INTENSITY = 3.2f;
constexpr float ENEMY_UNDER_LIGHT_INTENSITY = 2.6f;

#ifdef ASSET_DIR
constexpr const char* MODELS_DIR = ASSET_DIR "/models";
constexpr const char* TEXTURES_DIR = ASSET_DIR "/textures";
#else
constexpr const char* MODELS_DIR = "../../assets/models";
constexpr const char* TEXTURES_DIR = "../../assets/textures";
#endif

#ifndef SHADER_DIR
#define SHADER_DIR "../../shaders"
#endif

#ifdef SOURCE_DIR
constexpr const char* SWORDMAN_GLTF_PATH = SOURCE_DIR "/tests/loading_files/the_swordman/scene.gltf";
constexpr const char* SKELETON_GLTF_PATH = SOURCE_DIR "/assets/models/lowpoly_skeleton/scene.gltf";
#else
constexpr const char* SWORDMAN_GLTF_PATH = "tests/loading_files/the_swordman/scene.gltf";
constexpr const char* SKELETON_GLTF_PATH = "assets/models/lowpoly_skeleton/scene.gltf";
#endif

std::shared_ptr<TintedMaterial> makeTintedMaterial(
    AssetManager& assets,
    const std::string& name,
    std::shared_ptr<Shader> shader,
    const glm::vec4& tint)
{
    auto material = std::make_shared<TintedMaterial>();
    material->setShader(std::move(shader));
    material->tint = tint;
    assets.registerMaterial(name, material);
    return material;
}

std::shared_ptr<LitMaterial> makeLitFactorMaterial(
    AssetManager& assets,
    const std::string& name,
    std::shared_ptr<Shader> shader,
    const glm::vec3& albedo,
    const glm::vec3& specular,
    float roughness)
{
    auto material = std::make_shared<LitMaterial>(std::move(shader));
    material->setAlbedoFactor(albedo);
    material->setSpecularFactor(specular);
    material->setRoughnessFactor(roughness);
    material->setAmbientOcclusionFactor(1.0f);
    assets.registerMaterial(name, material);
    return material;
}

bool isDeadOrMissing(World& world, engine::ecs::EntityId entity)
{
    if (!world.registry().isAlive(entity)) {
        return true;
    }
    const auto* health = world.registry().get<HealthComponent>(entity);
    return health && health->dead;
}

glm::vec3 underLightPosition(World& world, engine::ecs::EntityId entity)
{
    glm::vec3 position = game::worldPosition(world, entity);
    position.y = CHARACTER_LIGHT_HEIGHT;
    return position;
}
} // namespace

namespace game {

GameApplication::~GameApplication()
{
    player_.reset();
    assets_ = GameAssets{};
    world_.shutdownGpuResources();
}

int GameApplication::run()
{
    Window window(WINDOW_W, WINDOW_H, "Cursed Crusade");
    if (!window.get_handle()) {
        return 1;
    }

    if (!GLContext::init()) {
        return 1;
    }
    GLContext::enable_default_render_settings();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_handle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    if (!loadAssets()) {
        player_.reset();
        assets_ = GameAssets{};
        world_.shutdownGpuResources();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        return 1;
    }

    setupLights();
    setupWorld();
    world_.frameScheduler().reset();

    uint64_t frameIndex = 0;
    while (!window.should_close()) {
        world_.frameScheduler().tick();
        float deltaTime = world_.frameScheduler().delta_time();
        if (deltaTime <= 0.0f) {
            deltaTime = 1.0f / 60.0f;
        }

        update(window, deltaTime);
        render(window, deltaTime, frameIndex++);
    }

    player_.reset();
    assets_ = GameAssets{};
    world_.shutdownGpuResources();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

bool GameApplication::loadAssets()
{
    AssetManager& assets = world_.assets();
    const std::string shaderDir = SHADER_DIR;
    const std::string textureDir = TEXTURES_DIR;
    const std::string modelDir = MODELS_DIR;

    assets_.mainShader = assets.loadShader(
        "game-main",
        shaderDir + "/blackToWhite.vert",
        shaderDir + "/blackToWhite.frag");
    assets_.houseShader = assets.loadShader(
        "game-textured",
        shaderDir + "/textured.vert",
        shaderDir + "/textured.frag");
    assets_.blendedShader = assets.loadShader(
        "game-blended",
        shaderDir + "/blended.vert",
        shaderDir + "/blended.frag");
    assets_.lightShader = assets.loadShader(
        "game-light",
        shaderDir + "/light.vert",
        shaderDir + "/light.frag");
    assets_.skinnedShader = assets.loadShader(
        "game-skinned",
        shaderDir + "/skinned.vert",
        shaderDir + "/skinned.frag");

    if (!assets_.mainShader || !assets_.houseShader || !assets_.blendedShader || !assets_.lightShader) {
        std::cerr << "Failed to load one or more required game shaders.\n";
        return false;
    }

    if (!assets_.skinnedShader) {
        std::cerr << "Warning: failed to load skinned shader; animated models will fall back to cubes.\n";
    }

    assets_.blue = makeTintedMaterial(assets, "game-blue", assets_.mainShader, {0.2f, 0.4f, 1.0f, 1.0f});
    assets_.brown = makeTintedMaterial(assets, "game-brown", assets_.mainShader, {0.5f, 0.2f, 0.1f, 1.0f});
    assets_.yellow = makeTintedMaterial(assets, "game-yellow", assets_.mainShader, {1.0f, 1.0f, 0.3f, 1.0f});
    assets_.green = makeTintedMaterial(assets, "game-green", assets_.mainShader, {0.4f, 1.0f, 0.2f, 1.0f});
    assets_.red = makeTintedMaterial(assets, "game-red", assets_.mainShader, {1.0f, 0.2f, 0.2f, 1.0f});
    assets_.xpGold = makeTintedMaterial(assets, "game-xp-gold", assets_.mainShader, {1.0f, 0.85f, 0.0f, 1.0f});

    auto moon = assets.loadTexture(textureDir + "/moon.jpg");
    auto houseTexture = assets.loadTexture(textureDir + "/house/house.jpeg");
    auto glassTexture = assets.loadTexture(textureDir + "/house/glass.png");
    auto asphaltSpecular = assets.loadTexture(textureDir + "/asphalt/specular.jpg");
    auto asphaltRoughness = assets.loadTexture(textureDir + "/asphalt/roughness.jpg");
    auto asphaltEmissive = assets.loadTexture(textureDir + "/asphalt/emissive.jpg");
    auto asphaltAlbedo = assets.loadTexture(textureDir + "/asphalt/albedo.jpg");
    auto suzanneAO = assets.loadTexture(textureDir + "/suzanne/ambient_occlusion.jpg");

    assets_.house = std::make_shared<TexturedMaterial>(assets_.houseShader, houseTexture);
    assets.registerMaterial("game-house", assets_.house);

    assets_.houseMixed = std::make_shared<TexturedMaterial>(assets_.blendedShader, houseTexture);
    assets_.houseMixed->addTextureLayer(moon, BlendMode::Lerp, 0.4f);
    assets.registerMaterial("game-house-mixed", assets_.houseMixed);

    assets_.glass = std::make_shared<TexturedMaterial>(assets_.blendedShader, glassTexture);
    assets.registerMaterial("game-glass", assets_.glass);

    assets_.asphalt = std::make_shared<LitMaterial>(
        assets_.lightShader,
        asphaltAlbedo,
        asphaltSpecular,
        asphaltRoughness,
        asphaltEmissive,
        suzanneAO);
    assets.registerMaterial("game-asphalt", assets_.asphalt);

    assets_.steel = makeLitFactorMaterial(
        assets,
        "game-lit-steel",
        assets_.lightShader,
        glm::vec3(0.72f, 0.76f, 0.82f),
        glm::vec3(0.85f),
        0.22f);
    assets_.brass = makeLitFactorMaterial(
        assets,
        "game-lit-brass",
        assets_.lightShader,
        glm::vec3(0.95f, 0.68f, 0.22f),
        glm::vec3(0.65f, 0.48f, 0.2f),
        0.32f);
    assets_.darkLeather = makeLitFactorMaterial(
        assets,
        "game-lit-dark-leather",
        assets_.lightShader,
        glm::vec3(0.16f, 0.075f, 0.035f),
        glm::vec3(0.08f),
        0.72f);

    Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));
    Mesh glassMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec2(1.0f));
    Mesh sphereMesh = Mesh::create_sphere();
    Mesh planeMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec2(1.0f));
    Mesh xpOrbMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(0.3f));
    Mesh swordBladeMesh = Mesh::create_cuboid(glm::vec3(0.0f, 0.0f, 1.14f), glm::vec3(0.055f, 0.028f, 1.65f));
    Mesh swordGuardMesh = Mesh::create_cuboid(glm::vec3(0.0f, 0.0f, 0.28f), glm::vec3(0.5f, 0.07f, 0.08f));
    Mesh swordGripMesh = Mesh::create_cuboid(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.07f, 0.07f, 0.44f));
    Mesh swordPommelMesh = Mesh::create_cuboid(glm::vec3(0.0f, 0.0f, -0.29f), glm::vec3(0.13f, 0.13f, 0.1f));

    assets_.cubeRenderer = assets.createMeshRenderer("game-cube", cubeMesh);
    assets_.glassRenderer = assets.createMeshRenderer("game-glass-plane", glassMesh);
    assets_.sphereRenderer = assets.createMeshRenderer("game-sphere", sphereMesh);
    assets_.planeRenderer = assets.createMeshRenderer("game-plane", planeMesh);
    assets_.xpOrbRenderer = assets.createMeshRenderer("game-xp-orb", xpOrbMesh);
    assets_.swordBladeRenderer = assets.createMeshRenderer("game-sword-blade", swordBladeMesh);
    assets_.swordGuardRenderer = assets.createMeshRenderer("game-sword-guard", swordGuardMesh);
    assets_.swordGripRenderer = assets.createMeshRenderer("game-sword-grip", swordGripMesh);
    assets_.swordPommelRenderer = assets.createMeshRenderer("game-sword-pommel", swordPommelMesh);

    const std::string houseMeshPath = modelDir + "/house/house.obj";
    assets_.houseMesh = assets.loadMesh(houseMeshPath);
    assets_.houseRenderer = assets.loadMeshRenderer(houseMeshPath);
    if (!assets_.houseRenderer) {
        std::cerr << "Warning: house.obj not found, using cube as fallback.\n";
        assets_.houseRenderer = assets_.cubeRenderer;
    }

    if (assets_.skinnedShader && std::filesystem::exists(SWORDMAN_GLTF_PATH)) {
        assets_.swordmanModel = assets.loadModel(SWORDMAN_GLTF_PATH);
        if (assets_.swordmanModel) {
            assets_.swordmanBaseAnimation = assets_.swordmanModel->animations.empty() ? -1 : 0;
            assets_.swordmanAttackAnimation = ensurePlayerAttackAnimation(*assets_.swordmanModel);
            assets_.swordmanRenderers = assets.createSkinnedMeshRenderers("game-swordman", *assets_.swordmanModel);
        }
    }

    if (assets_.skinnedShader && std::filesystem::exists(SKELETON_GLTF_PATH)) {
        assets_.skeletonModel = assets.loadModel(SKELETON_GLTF_PATH);
        if (assets_.skeletonModel) {
            assets_.skeletonRenderers = assets.createSkinnedMeshRenderers("game-skeleton", *assets_.skeletonModel);
        }
    }

    return true;
}

void GameApplication::setupLights()
{
    world_.lights().initUBO();
}

void GameApplication::setupWorld()
{
    GameEntityFactory factory(world_);

    world_.get_camera().position = glm::vec3(10.0f, 5.0f, 10.0f);
    world_.get_camera().direction = glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f));
    world_.get_camera().up = glm::vec3(0.0f, 1.0f, 0.0f);
    world_.get_camera().fov = glm::radians(60.0f);
    world_.get_camera().near = 0.1f;
    world_.get_camera().far = 100.0f;

    state_.root = factory.createNode("SceneRoot");
    state_.island = factory.createNode("Island", state_.root);

    player_ = std::make_unique<Crusader>(
        world_,
        state_.root,
        assets_.cubeRenderer,
        assets_.green,
        assets_.cubeRenderer,
        assets_.brown);
    state_.player = player_->entity();
    player_->setPosition(glm::vec3(0.0f));
    player_->attachCamera(&world_.get_camera(), glm::vec3(0.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    state_.progress.moveSpeed = 4.0f;
    state_.progress.dodgeSpeed = 10.0f;
    state_.progress.damage = 25;
    player_->setMoveSpeed(state_.progress.moveSpeed);
    player_->setDodgeSpeed(state_.progress.dodgeSpeed);

    auto& playerHealth = world_.registry().emplace<HealthComponent>(state_.player);
    playerHealth.maxHP = 100;
    playerHealth.hp = playerHealth.maxHP;
    playerHealth.invulnDuration = 0.5f;
    playerHealth.respawnDelay = 3.0f;
    playerHealth.set_spawn_point(player_->getPosition());

    auto& playerHurtbox = world_.registry().emplace<HurtboxComponent>(state_.player);
    playerHurtbox.halfExtents = {0.4f, 0.9f, 0.4f};
    playerHurtbox.localOffset = {0.0f, 1.0f, 0.0f};

    auto& playerCombat = world_.registry().emplace<CombatComponent>(state_.player);
    playerCombat.damage = state_.progress.damage;
    playerCombat.hitbox.halfExtents = {0.6f, 0.8f, 0.6f};
    playerCombat.hitbox.localOffset = {0.0f, 1.0f, 0.9f};

    auto& playerCollider = world_.registry().emplace<engine::ecs::ColliderData>(state_.player);
    playerCollider.halfExtents = glm::vec3(0.3f, 0.45f, 0.3f);
    playerCollider.localOffset = glm::vec3(0.0f, 1.0f, 0.0f);

    if (assets_.swordmanModel && !assets_.swordmanRenderers.empty()) {
        const float swordmanScale = 0.01f;
        auto material = makeSkinnedMaterial(assets_.swordmanModel);
        state_.playerVisual = factory.createSkinnedRenderable(
            "Swordman",
            assets_.swordmanRenderers,
            material,
            assets_.swordmanModel,
            state_.player,
            glm::vec3(0.0f),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3(swordmanScale));

        auto& animation = world_.registry().emplace<engine::ecs::AnimatorData>(state_.playerVisual);
        animation.model = assets_.swordmanModel;
        animation.skinIndex = 0;
        animation.currentAnimation = assets_.swordmanBaseAnimation >= 0
            ? assets_.swordmanBaseAnimation
            : assets_.swordmanAttackAnimation;
        animation.playing = animation.currentAnimation >= 0;
        animation.loop = assets_.swordmanBaseAnimation >= 0;

        world_.registry().remove<engine::ecs::Renderable>(player_->getBody());
        attachSwordToPlayerHand();
    }

    state_.testHouse = factory.createStaticRenderable(
        "House",
        assets_.houseRenderer,
        assets_.houseMixed,
        state_.root,
        glm::vec3(10.0f, 1.0f, 1.0f));

    factory.createStaticRenderable(
        "Asphalt",
        assets_.planeRenderer,
        assets_.asphalt,
        state_.root,
        glm::vec3(0.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(100.0f));

    state_.collisionSphere = factory.createStaticRenderable(
        "CollisionSphere",
        assets_.sphereRenderer,
        assets_.asphalt,
        state_.root,
        glm::vec3(4.0f, 2.0f, -3.0f),
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec3(0.6f));

    auto& houseCollider = world_.registry().emplace<engine::ecs::ColliderData>(state_.testHouse);
    if (assets_.houseMesh) {
        PhysicsCollisionSystem::MeshBounds bounds;
        if (PhysicsCollisionSystem::computeMeshBounds(*assets_.houseMesh, bounds)) {
            houseCollider.localOffset = bounds.center;
            houseCollider.halfExtents = bounds.halfExtents;
        }
    } else {
        houseCollider.halfExtents = glm::vec3(0.5f);
    }

    auto& sphereCollider = world_.registry().emplace<engine::ecs::ColliderData>(state_.collisionSphere);
    sphereCollider.halfExtents = glm::vec3(1.0f);

    spawnInitialWave();
    TransformSystem::updateWorldTransforms(world_.registry());
    updateCharacterLights();
}

void GameApplication::attachSwordToPlayerHand()
{
    if (!assets_.swordmanModel ||
        assets_.swordmanModel->skins.empty() ||
        !assets_.swordmanModel->skins.front().skeleton ||
        !world_.registry().isAlive(state_.playerVisual)) {
        return;
    }

    const Skeleton& skeleton = *assets_.swordmanModel->skins.front().skeleton;
    engine::ecs::BoneAttachment attachment = makePlayerSwordAttachment(state_.playerVisual, skeleton);
    if (attachment.boneName.empty()) {
        std::cerr << "Warning: swordman right-hand bone was not found; keeping fallback weapon.\n";
        return;
    }

    if (state_.playerSword.valid() && world_.registry().isAlive(state_.playerSword)) {
        world_.destroyEntity(state_.playerSword, DestroyMode::Recursive);
    }

    if (player_ && world_.registry().isAlive(player_->getWeapon())) {
        world_.destroyEntity(player_->getWeapon(), DestroyMode::Recursive);
    }

    SwordVisualAssets swordAssets;
    swordAssets.bladeRenderer = assets_.swordBladeRenderer;
    swordAssets.guardRenderer = assets_.swordGuardRenderer;
    swordAssets.gripRenderer = assets_.swordGripRenderer;
    swordAssets.pommelRenderer = assets_.swordPommelRenderer;
    swordAssets.bladeMaterial = assets_.steel;
    swordAssets.guardMaterial = assets_.brass;
    swordAssets.gripMaterial = assets_.darkLeather;

    state_.playerSword = createPlayerSword(world_, state_.player, swordAssets);
    world_.registry().emplace<engine::ecs::BoneAttachment>(state_.playerSword, attachment);

    if (auto* controller = world_.registry().get<PlayerController>(state_.player)) {
        controller->weapon = state_.playerSword;
        if (const auto* transform = world_.registry().get<engine::ecs::Transform>(state_.playerSword)) {
            controller->weaponRestRotation = transform->rotation;
        }
    }
}

void GameApplication::spawnInitialWave()
{
    state_.waves.currentWave = 1;
    state_.waves.enemiesPerWave = 1;
    state_.waves.enemyDamageMultiplier = 1;
    spawnEnemy(glm::vec3(-4.0f, 0.0f, 0.0f));
    state_.waves.enemiesAlive = 1;
}

engine::ecs::EntityId GameApplication::spawnEnemy(const glm::vec3& position)
{
    auto enemy = CreateEnemy(world_, state_.root, assets_.cubeRenderer, assets_.red);
    engine::ecs::EntityId enemyEntity = enemy->entity();
    enemy->setPosition(position);
    enemy->setTarget(state_.player);
    enemy->getCombat().damage = 15 * state_.waves.enemyDamageMultiplier;

    if (auto* health = world_.registry().get<HealthComponent>(enemyEntity)) {
        health->set_spawn_point(position);
    }

    if (assets_.skeletonModel && !assets_.skeletonRenderers.empty()) {
        const float skeletonScale = 0.01f / 6.0f;
        const glm::quat skeletonRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        engine::ecs::EntityId visual = world_.createSkinnedRenderable(
            "Skeleton",
            assets_.skeletonRenderers,
            makeSkinnedMaterial(assets_.skeletonModel),
            assets_.skeletonModel,
            enemyEntity,
            glm::vec3(0.0f),
            skeletonRotation,
            glm::vec3(skeletonScale));

        auto& animation = world_.registry().emplace<engine::ecs::AnimatorData>(visual);
        animation.model = assets_.skeletonModel;
        animation.skinIndex = 0;
        animation.currentAnimation = 0;
        animation.playing = true;
        animation.loop = true;

        world_.registry().remove<engine::ecs::Renderable>(enemy->getBody());
    }

    state_.enemies.push_back(enemyEntity);
    return enemyEntity;
}

void GameApplication::spawnXpBurst(const glm::vec3& position)
{
    const int count = 3 + std::rand() % 3;
    for (int i = 0; i < count; ++i) {
        engine::ecs::EntityId orb = world_.createRenderable(
            "XPOrb",
            assets_.xpOrbRenderer,
            assets_.xpGold,
            state_.root,
            position + glm::vec3(0.0f, 1.0f, 0.0f));

        const float angle = static_cast<float>(std::rand() % 360) * 3.1415926f / 180.0f;
        const float speed = 2.0f + static_cast<float>(std::rand() % 100) / 50.0f;
        auto& xp = world_.registry().emplace<XPOrbComponent>(orb);
        xp.velocity = glm::vec3(
            std::cos(angle) * speed * 0.5f,
            4.0f + static_cast<float>(std::rand() % 100) / 50.0f,
            std::sin(angle) * speed * 0.5f);
        xp.xpValue = 10 + std::rand() % 15;
        xp.lifetime = 15.0f;
        world_.registry().emplace<XpOrbTag>(orb);
        state_.xpOrbs.push_back(orb);
    }
}

void GameApplication::update(Window& window, float deltaTime)
{
    window.poll_events();

    if (auto* island = world_.transform(state_.island)) {
        const glm::quat deltaRot =
            glm::angleAxis(glm::radians(30.0f) * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        island->rotation = deltaRot * island->rotation;
        island->dirty = true;
    }

    PlayerInput input;
    auto* playerHealth = world_.registry().get<HealthComponent>(state_.player);
    if (!playerHealth || !playerHealth->dead) {
        GLFWwindow* handle = window.get_handle();
        if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS) input.move.y -= 1.0f;
        if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS) input.move.y += 1.0f;
        if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS) input.move.x += 1.0f;
        if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS) input.move.x -= 1.0f;
        input.block = glfwGetKey(handle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
        input.attack =
            glfwGetMouseButton(handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
            glfwGetKey(handle, GLFW_KEY_J) == GLFW_PRESS;
        input.dodge = glfwGetKey(handle, GLFW_KEY_SPACE) == GLFW_PRESS;
    }

    if (player_) {
        player_->setInput(input);
        player_->update(deltaTime);
    }

    if (playerHealth) {
        playerHealth->update(deltaTime);
        if (playerHealth->ready_to_respawn()) {
            playerHealth->respawn();
            if (player_) {
                player_->setPosition(playerHealth->spawnPos);
            }
        }
    }

    for (engine::ecs::EntityId enemy : state_.enemies) {
        if (!world_.registry().isAlive(enemy)) {
            continue;
        }
        if (auto* health = world_.registry().get<HealthComponent>(enemy)) {
            health->update(deltaTime);
            if (!health->dead) {
                updateEnemyAI(world_, enemy, deltaTime);
            }
        }
    }

    TransformSystem::updateWorldTransforms(world_.registry());
    updateCombat(deltaTime);
    updateXpOrbs(deltaTime);
    updateWaves();
    updateCollisions();
    updatePlayerAnimationState();
    animationSystem_.update(world_.registry(), deltaTime);
    boneAttachmentSystem_.update(world_.registry(), deltaTime);
    world_.systems().updateAll(world_.registry(), deltaTime);
    TransformSystem::updateWorldTransforms(world_.registry());
    updateCharacterLights();
}

void GameApplication::updateCharacterLights()
{
    LightSystem& lights = world_.lights();
    lights.clearLights();

    int remainingLights = LightSystem::MAX_LIGHTS;
    auto addLight = [&lights, &remainingLights](const Light& light) {
        if (remainingLights <= 0) {
            return;
        }
        lights.addLight(light);
        --remainingLights;
    };

    addLight(Light(
        LightType::DIRECTIONAL,
        glm::vec3(1.0f),
        glm::vec3(0.0f, 10.0f, 0.0f),
        glm::normalize(glm::vec3(0.2f, -1.0f, 0.15f)),
        glm::radians(15.0f),
        glm::radians(30.0f),
        0.45f));

    if (!isDeadOrMissing(world_, state_.player)) {
        addLight(Light(
            LightType::POINT,
            glm::vec3(0.12f, 0.45f, 1.0f),
            underLightPosition(world_, state_.player),
            glm::vec3(0.0f),
            glm::radians(15.0f),
            glm::radians(30.0f),
            PLAYER_UNDER_LIGHT_INTENSITY));
    }

    for (engine::ecs::EntityId enemy : state_.enemies) {
        if (isDeadOrMissing(world_, enemy)) {
            continue;
        }

        addLight(Light(
            LightType::POINT,
            glm::vec3(1.0f, 0.08f, 0.05f),
            underLightPosition(world_, enemy),
            glm::vec3(0.0f),
            glm::radians(15.0f),
            glm::radians(30.0f),
            ENEMY_UNDER_LIGHT_INTENSITY));
    }
}

void GameApplication::updatePlayerAnimationState()
{
    if (assets_.swordmanAttackAnimation < 0 || !world_.registry().isAlive(state_.playerVisual)) {
        return;
    }

    auto* animation = world_.registry().get<engine::ecs::AnimatorData>(state_.playerVisual);
    if (!animation) {
        return;
    }

    const bool attacking = player_ && player_->isAttacking();
    if (attacking && !playerAttackAnimationActive_) {
        animation->currentAnimation = assets_.swordmanAttackAnimation;
        animation->loop = false;
        animation->speed = 1.0f;
        animation->playing = true;
        animation->animator.stop();
        playerAttackAnimationActive_ = true;
        return;
    }

    if (!attacking && playerAttackAnimationActive_) {
        const int fallbackAnimation = assets_.swordmanBaseAnimation >= 0
            ? assets_.swordmanBaseAnimation
            : assets_.swordmanAttackAnimation;
        animation->currentAnimation = fallbackAnimation;
        animation->loop = assets_.swordmanBaseAnimation >= 0;
        animation->speed = 1.0f;
        animation->playing = fallbackAnimation >= 0;
        animation->animator.stop();
        playerAttackAnimationActive_ = false;
    }
}

void GameApplication::updateCombat(float)
{
    auto* playerHealth = world_.registry().get<HealthComponent>(state_.player);
    auto* playerHurtbox = world_.registry().get<HurtboxComponent>(state_.player);
    auto* playerCombat = world_.registry().get<CombatComponent>(state_.player);
    if (!playerHealth || !playerHurtbox || !playerCombat) {
        return;
    }

    const bool playerAttacking = player_ && player_->isAttacking() && !playerHealth->dead;
    const glm::vec3 playerPos = worldPosition(world_, state_.player);
    const glm::vec3 playerForward = forward(world_, state_.player);
    const DefenseState playerDefense = defenseState(world_, state_.player);

    for (engine::ecs::EntityId& enemy : state_.enemies) {
        if (!world_.registry().isAlive(enemy)) {
            continue;
        }

        auto* ai = world_.registry().get<EnemyAI>(enemy);
        auto* enemyHealth = world_.registry().get<HealthComponent>(enemy);
        auto* enemyHurtbox = world_.registry().get<HurtboxComponent>(enemy);
        auto* enemyCombat = world_.registry().get<CombatComponent>(enemy);
        if (!ai || !enemyHealth || !enemyHurtbox || !enemyCombat) {
            continue;
        }

        if (!enemyHealth->dead) {
            playerCombat->resolve_attack(
                playerAttacking,
                playerPos,
                playerForward,
                *enemyHurtbox,
                worldPosition(world_, enemy),
                *enemyHealth);
        }

        if (enemyHealth->dead) {
            if (!ai->wasDead) {
                ai->wasDead = true;
                spawnXpBurst(worldPosition(world_, enemy));
                state_.waves.totalEnemiesKilled++;
                std::cout << "Enemy killed! Total: " << state_.waves.totalEnemiesKilled << std::endl;
            }
            world_.destroyEntity(enemy, DestroyMode::Recursive);
            enemy = engine::ecs::InvalidEntity;
            continue;
        }

        if (!playerHealth->dead) {
            enemyCombat->resolve_attack(
                ai->attackTimer > 0.0f,
                worldPosition(world_, enemy),
                forward(world_, enemy),
                *playerHurtbox,
                playerPos,
                *playerHealth,
                &playerDefense);
        }
    }
}

void GameApplication::updateWaves()
{
    state_.enemies.erase(
        std::remove_if(
            state_.enemies.begin(),
            state_.enemies.end(),
            [this](engine::ecs::EntityId enemy) {
                return !world_.registry().isAlive(enemy);
            }),
        state_.enemies.end());

    state_.waves.enemiesAlive = 0;
    for (engine::ecs::EntityId enemy : state_.enemies) {
        if (!isDeadOrMissing(world_, enemy)) {
            state_.waves.enemiesAlive++;
        }
    }

    if (state_.waves.enemiesAlive != 0 || !state_.waves.waveInProgress) {
        return;
    }

    state_.waves.currentWave++;
    state_.waves.enemiesPerWave++;
    if (state_.waves.enemiesPerWave > 5) {
        state_.waves.enemiesPerWave = 1;
        state_.waves.enemyDamageMultiplier *= 2;
    }

    const glm::vec3 playerPos = worldPosition(world_, state_.player);
    std::cout << "WAVE " << state_.waves.currentWave << " START! Spawning "
              << state_.waves.enemiesPerWave << " enemies. DMG x"
              << state_.waves.enemyDamageMultiplier << std::endl;

    for (int i = 0; i < state_.waves.enemiesPerWave; ++i) {
        const float angle = static_cast<float>(i) / static_cast<float>(state_.waves.enemiesPerWave) * 6.28318f;
        const float radius = 5.0f + static_cast<float>(std::rand() % 30) / 10.0f;
        spawnEnemy(playerPos + glm::vec3(std::cos(angle) * radius, 0.0f, std::sin(angle) * radius));
    }
    state_.waves.enemiesAlive = state_.waves.enemiesPerWave;
}

void GameApplication::updateXpOrbs(float deltaTime)
{
    const glm::vec3 playerPos = worldPosition(world_, state_.player);
    constexpr float pickupRadius = 1.5f;
    constexpr float gravity = 9.8f;

    for (engine::ecs::EntityId& orb : state_.xpOrbs) {
        auto* transform = world_.registry().get<engine::ecs::Transform>(orb);
        auto* xp = world_.registry().get<XPOrbComponent>(orb);
        if (!transform || !xp) {
            orb = engine::ecs::InvalidEntity;
            continue;
        }

        xp->velocity.y -= gravity * deltaTime;
        transform->position += xp->velocity * deltaTime;
        if (transform->position.y < 0.5f) {
            transform->position.y = 0.5f;
            xp->velocity.y = 0.0f;
            xp->velocity.x *= 0.8f;
            xp->velocity.z *= 0.8f;
        }

        transform->rotation =
            glm::angleAxis(deltaTime * 3.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            transform->rotation;
        transform->dirty = true;

        xp->lifetime -= deltaTime;
        const bool pickedUp = glm::length(transform->position - playerPos) < pickupRadius;
        if (pickedUp) {
            grantExperience(state_.progress, xp->xpValue);
        }

        if (pickedUp || xp->lifetime <= 0.0f) {
            world_.destroyEntity(orb, DestroyMode::Recursive);
            orb = engine::ecs::InvalidEntity;
        }
    }

    state_.xpOrbs.erase(
        std::remove_if(
            state_.xpOrbs.begin(),
            state_.xpOrbs.end(),
            [this](engine::ecs::EntityId orb) {
                return !world_.registry().isAlive(orb);
            }),
        state_.xpOrbs.end());
}

void GameApplication::updateCollisions()
{
    TransformSystem::updateWorldTransforms(world_.registry());

    auto* playerHealth = world_.registry().get<HealthComponent>(state_.player);
    if (playerHealth && !playerHealth->dead) {
        PhysicsCollisionSystem::resolveStaticCollision(world_.registry(), state_.player, state_.testHouse);
        PhysicsCollisionSystem::resolveStaticCollision(world_.registry(), state_.player, state_.collisionSphere);
    }

    PhysicsCollisionSystem::resolveStaticCollision(world_.registry(), state_.collisionSphere, state_.testHouse);

    for (engine::ecs::EntityId enemy : state_.enemies) {
        if (world_.registry().isAlive(enemy)) {
            PhysicsCollisionSystem::resolveStaticCollision(world_.registry(), state_.player, enemy);
        }
    }
}

void GameApplication::render(Window& window, float deltaTime, uint64_t frameIndex)
{
    int framebufferWidth = 1;
    int framebufferHeight = 1;
    window.get_framebuffer_size(framebufferWidth, framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::vec2 viewportSize(
        framebufferWidth > 0 ? static_cast<float>(framebufferWidth) : 1.0f,
        framebufferHeight > 0 ? static_cast<float>(framebufferHeight) : 1.0f);
    const glm::mat4 view = world_.get_camera().get_view_matrix();
    const glm::mat4 projection = world_.get_camera().get_projection_matrix(viewportSize);
    const glm::mat4 viewProjection = projection * view;

    RenderContext context;
    context.view = view;
    context.projection = projection;
    context.viewProj = viewProjection;
    context.cameraPosition = world_.get_camera().position;
    context.deltaTime = deltaTime;
    context.frameIndex = frameIndex;
    context.debugMode = renderDebugMode_;

    world_.renderer().setDebugMode(renderDebugMode_);
    world_.renderer().render(world_, context);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    renderUi(viewProjection, framebufferWidth, framebufferHeight);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    window.swap_buffers();
}

void GameApplication::renderUi(const glm::mat4& viewProjection, int width, int height)
{
    for (engine::ecs::EntityId enemy : state_.enemies) {
        auto* health = world_.registry().get<HealthComponent>(enemy);
        auto* hurtbox = world_.registry().get<HurtboxComponent>(enemy);
        auto* transform = world_.registry().get<engine::ecs::Transform>(enemy);
        if (health && hurtbox && transform && !health->dead) {
            HealthBarSystem::draw_over_entity(*health, *hurtbox, *transform, viewProjection, width, height);
        }
    }

    ImGui::Begin("Cursed Crusade");
    const char* debugModes[] = {
        "Final",
        "Albedo",
        "Normals",
        "Lighting",
        "Bones",
        "Texture Channels"
    };
    int debugModeIndex = static_cast<int>(renderDebugMode_);
    if (ImGui::Combo("Render Debug", &debugModeIndex, debugModes, IM_ARRAYSIZE(debugModes))) {
        renderDebugMode_ = static_cast<RenderDebugMode>(debugModeIndex);
    }

    ImGuiHelpers::ShowTransformInspector("Selected", world_.transform(state_.testHouse));
    if (player_) {
        const glm::vec3 pos = player_->getPosition();
        ImGui::Separator();
        ImGui::Text("Player pos: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
        ImGui::Text("Block: %s", player_->isBlocking() ? "yes" : "no");
        ImGui::Text("Attack: %s", player_->isAttacking() ? "yes" : "no");
        ImGui::Text("Dodge: %s", player_->isDodging() ? "yes" : "no");
    }
    ImGui::Separator();
    ImGui::Text("Enemies: %d alive", state_.waves.enemiesAlive);
    int index = 1;
    for (engine::ecs::EntityId enemy : state_.enemies) {
        const auto* health = world_.registry().get<HealthComponent>(enemy);
        if (!health) {
            continue;
        }
        const float hpFrac = health->maxHP > 0
            ? glm::clamp(static_cast<float>(health->hp) / static_cast<float>(health->maxHP), 0.0f, 1.0f)
            : 0.0f;
        ImGui::Text("Enemy %d", index++);
        ImGui::ProgressBar(hpFrac, ImVec2(0.0f, 0.0f));
    }
    ImGui::End();

    renderOverlayBars();
    renderUpgradeMenu();
}

void GameApplication::renderOverlayBars()
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const float barWidth = 200.0f;
    const float padding = 20.0f;

    const auto* playerHealth = world_.registry().get<HealthComponent>(state_.player);
    const float hpFrac = playerHealth && playerHealth->maxHP > 0
        ? glm::clamp(static_cast<float>(playerHealth->hp) / static_cast<float>(playerHealth->maxHP), 0.0f, 1.0f)
        : 0.0f;

    ImVec2 hpPos(io.DisplaySize.x - barWidth - padding, padding);
    ImVec2 hpEnd(hpPos.x + barWidth, hpPos.y + 25.0f);
    drawList->AddRectFilled(hpPos, hpEnd, IM_COL32(40, 40, 40, 200), 4.0f);
    const ImU32 hpColor = hpFrac > 0.5f ? IM_COL32(80, 200, 80, 220)
        : hpFrac > 0.25f ? IM_COL32(220, 180, 50, 220)
        : IM_COL32(220, 60, 60, 220);
    drawList->AddRectFilled(hpPos, ImVec2(hpPos.x + barWidth * hpFrac, hpEnd.y), hpColor, 4.0f);
    drawList->AddRect(hpPos, hpEnd, IM_COL32(255, 255, 255, 180), 4.0f, 0, 2.0f);

    char hpText[32];
    std::snprintf(
        hpText,
        sizeof(hpText),
        "HP: %d / %d",
        playerHealth ? playerHealth->hp : 0,
        playerHealth ? playerHealth->maxHP : 0);
    ImVec2 hpTextSize = ImGui::CalcTextSize(hpText);
    drawList->AddText(
        ImVec2(hpPos.x + (barWidth - hpTextSize.x) * 0.5f, hpPos.y + (25.0f - hpTextSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        hpText);

    if (playerHealth && playerHealth->dead) {
        const char* deadText = "RESPAWNING...";
        ImVec2 deadSize = ImGui::CalcTextSize(deadText);
        drawList->AddText(
            ImVec2(hpPos.x + (barWidth - deadSize.x) * 0.5f, hpEnd.y + 5.0f),
            IM_COL32(255, 100, 100, 255),
            deadText);
    }

    ImVec2 xpPos(io.DisplaySize.x - barWidth - padding, padding + 55.0f);
    ImVec2 xpEnd(xpPos.x + barWidth, xpPos.y + 20.0f);
    const float xpFrac = state_.progress.xpToNextLevel > 0
        ? glm::clamp(
              static_cast<float>(state_.progress.xp) / static_cast<float>(state_.progress.xpToNextLevel),
              0.0f,
              1.0f)
        : 0.0f;
    drawList->AddRectFilled(xpPos, xpEnd, IM_COL32(40, 40, 40, 200), 4.0f);
    drawList->AddRectFilled(xpPos, ImVec2(xpPos.x + barWidth * xpFrac, xpEnd.y), IM_COL32(255, 215, 0, 220), 4.0f);
    drawList->AddRect(xpPos, xpEnd, IM_COL32(255, 255, 255, 180), 4.0f, 0, 2.0f);

    char xpText[48];
    std::snprintf(
        xpText,
        sizeof(xpText),
        "LVL %d | XP: %d / %d",
        state_.progress.level,
        state_.progress.xp,
        state_.progress.xpToNextLevel);
    ImVec2 xpTextSize = ImGui::CalcTextSize(xpText);
    drawList->AddText(
        ImVec2(xpPos.x + (barWidth - xpTextSize.x) * 0.5f, xpPos.y + (20.0f - xpTextSize.y) * 0.5f),
        IM_COL32(255, 255, 255, 255),
        xpText);

    char waveText[64];
    std::snprintf(
        waveText,
        sizeof(waveText),
        "Wave %d | Enemies: %d/%d | DMG: x%d",
        state_.waves.currentWave,
        state_.waves.enemiesAlive,
        state_.waves.enemiesPerWave,
        state_.waves.enemyDamageMultiplier);
    ImVec2 waveSize = ImGui::CalcTextSize(waveText);
    drawList->AddText(
        ImVec2(xpPos.x + (barWidth - waveSize.x) * 0.5f, xpEnd.y + 5.0f),
        IM_COL32(200, 200, 200, 255),
        waveText);
}

void GameApplication::renderUpgradeMenu()
{
    if (!state_.progress.showUpgradeMenu || state_.progress.pendingUpgrades <= 0) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(300.0f, 200.0f);
    ImVec2 windowPos((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::Begin(
        "LEVEL UP!",
        nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Choose an upgrade! (%d available)", state_.progress.pendingUpgrades);
    ImGui::Separator();

    char speedText[64];
    std::snprintf(
        speedText,
        sizeof(speedText),
        "Speed (%.1f -> %.1f)",
        state_.progress.moveSpeed,
        state_.progress.moveSpeed + 0.5f);
    if (ImGui::Button(speedText, ImVec2(280.0f, 40.0f))) {
        state_.progress.moveSpeed += 0.5f;
        if (player_) {
            player_->setMoveSpeed(state_.progress.moveSpeed);
        }
        state_.progress.pendingUpgrades--;
    }

    char damageText[64];
    std::snprintf(
        damageText,
        sizeof(damageText),
        "Damage (%d -> %d)",
        state_.progress.damage,
        state_.progress.damage + 5);
    if (ImGui::Button(damageText, ImVec2(280.0f, 40.0f))) {
        state_.progress.damage += 5;
        if (auto* combat = world_.registry().get<CombatComponent>(state_.player)) {
            combat->damage = state_.progress.damage;
        }
        state_.progress.pendingUpgrades--;
    }

    char dodgeText[64];
    std::snprintf(
        dodgeText,
        sizeof(dodgeText),
        "Agility (%.1f -> %.1f)",
        state_.progress.dodgeSpeed,
        state_.progress.dodgeSpeed + 1.0f);
    if (ImGui::Button(dodgeText, ImVec2(280.0f, 40.0f))) {
        state_.progress.dodgeSpeed += 1.0f;
        if (player_) {
            player_->setDodgeSpeed(state_.progress.dodgeSpeed);
        }
        state_.progress.pendingUpgrades--;
    }

    if (state_.progress.pendingUpgrades <= 0) {
        state_.progress.showUpgradeMenu = false;
    }

    ImGui::End();
}

std::shared_ptr<SkinnedMaterial> GameApplication::makeSkinnedMaterial(const std::shared_ptr<ModelAsset>& model) const
{
    std::shared_ptr<Texture> texture;
    if (model && !model->textures.empty()) {
        texture = model->textures.front().texture;
    }

    auto material = std::make_shared<SkinnedMaterial>(assets_.skinnedShader, texture);
    material->set_animated(true);
    return material;
}

} // namespace game
