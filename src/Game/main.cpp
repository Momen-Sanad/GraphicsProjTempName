#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../engine/core/Application.hpp"
#include "../engine/assets/AssetManager.hpp"
#include "../engine/assets/LitMaterial.hpp"
#include "../engine/assets/MeshLoader.hpp"
#include "../engine/assets/TexturedMaterial.hpp"
#include "../engine/assets/TextureLoader.hpp"
#include "../engine/assets/TintedMaterial.hpp"
#include "../engine/components/MeshRenderer.hpp"
#include "../engine/components/Collider.hpp"
#include "../engine/gl/GpuMesh.hpp"
#include "../engine/gl/Mesh.hpp"
#include "../engine/systems/PhysicsCollisionSystem.hpp"
#include "../engine/utils/Im_GUI_Inspector.hpp"
#include "../engine/platform/Input.hpp"

#include "Entities/Crusader.hpp"
#include "Entities/Enemy.hpp"
#include "Entities/Player.hpp"

#define WINDOW_W 1280
#define WINDOW_H 720

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

class GameApp final : public GameLayer {
public:
    void onInit(Engine& engine) override {
        world = &engine.world();
        window = &engine.window();
        windowHandle = window->get_handle();

        initImGui();
        loadAssets(engine);
        buildScene();

        lastFpsTime = static_cast<float>(glfwGetTime());
        frameCount = 0;
    }

    void onUpdate(Engine&, float deltaSeconds) override {
        float time = static_cast<float>(glfwGetTime());
        frameCount++;

        if (time - lastFpsTime >= 1.0f) {
            if (island) {
                glm::vec3 pos = island->getPosition();
                glm::quat rot = island->getRotation();
                std::cout << "Island position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
                std::cout << "Island rotation: " << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << std::endl;
            }
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastFpsTime = time;
        }

        if (water) {
            water->setScale({10.f, 1.0f + 0.1f * glm::sin(time), 10.f});
        }

        if (island) {
            glm::quat deltaRot = glm::angleAxis(glm::radians(30.f) * deltaSeconds, glm::vec3(0.f, 1.f, 0.f));
            island->setRotation(deltaRot * island->getRotation());
        }

        updatePlayer(deltaSeconds);
        updateCollisionSphere(deltaSeconds);
        resolveCollisions();
    }

    void onRender(Engine&, float) override {
        if (!imguiReady) {
            return;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Hello ImGui");
        ImGui::Text("This is working!");

        if (testhouse) {
            ImGuiHelpers::ShowTransformInspector("Selected", testhouse);
        }

        if (player) {
            ImGui::Separator();
            glm::vec3 pos = player->getPosition();
            ImGui::Text("Player pos: %.2f %.2f %.2f", pos.x, pos.y, pos.z);
            ImGui::Text("Block: %s", player->isBlocking() ? "yes" : "no");
            ImGui::Text("Attack: %s", player->isAttacking() ? "yes" : "no");
            ImGui::Text("Dodge: %s", player->isDodging() ? "yes" : "no");
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void onShutdown(Engine&) override {
        if (!imguiReady) {
            return;
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        imguiReady = false;
    }

private:
    void initImGui() {
        if (!windowHandle) {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(windowHandle, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        imguiReady = true;
    }

    void loadAssets(Engine& engine) {
        auto& shaders = engine.assets().shaders();
        mainShader = shaders.loadShader("main",
                                        std::string(SHADER_DIR) + "/blackToWhite.vert",
                                        std::string(SHADER_DIR) + "/blackToWhite.frag");
        houseShader = shaders.loadShader("textured",
                                         std::string(SHADER_DIR) + "/textured.vert",
                                         std::string(SHADER_DIR) + "/textured.frag");
        houseMixedShader = shaders.loadShader("blended",
                                              std::string(SHADER_DIR) + "/blended.vert",
                                              std::string(SHADER_DIR) + "/blended.frag");

        if (!mainShader) {
            std::cerr << "Failed to load main shader" << std::endl;
        }

        blue.setShader(mainShader);
        blue.tint = glm::vec4(0.2f, 0.4f, 1.0f, 1.0f);

        brown.setShader(mainShader);
        brown.tint = glm::vec4(0.5f, 0.2f, 0.1f, 1.0f);

        yellow.setShader(mainShader);
        yellow.tint = glm::vec4(1.0f, 1.0f, 0.3f, 1.0f);

        green.setShader(mainShader);
        green.tint = glm::vec4(0.4f, 1.0f, 0.2f, 1.0f);

        red.setShader(mainShader);
        red.tint = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);

        moonTexture = TextureLoader::load(std::string(TEXTURES_DIR) + "/moon.jpg");
        houseTexture = TextureLoader::load(std::string(TEXTURES_DIR) + "/house/house.jpeg");
        glassTexture = TextureLoader::load(std::string(TEXTURES_DIR) + "/house/glass.png");
        skyTexture = TextureLoader::load(std::string(TEXTURES_DIR) + "/sky.jpg");

        asphaltSpecular = TextureLoader::load(std::string(TEXTURES_DIR) + "/asphalt/specular.jpg");
        asphaltRoughness = TextureLoader::load(std::string(TEXTURES_DIR) + "/asphalt/roughness.jpg");
        asphaltEmissive = TextureLoader::load(std::string(TEXTURES_DIR) + "/asphalt/emissive.jpg");
        asphaltAlbedo = TextureLoader::load(std::string(TEXTURES_DIR) + "/asphalt/albedo.jpg");

        houseMaterial = std::make_unique<TexturedMaterial>(houseShader, houseTexture);
        houseMixedMaterial = std::make_unique<TexturedMaterial>(houseMixedShader, houseTexture);
        houseMixedMaterial->addTextureLayer(moonTexture, BlendMode::Lerp, 0.4f);

        skyMaterial = std::make_unique<TexturedMaterial>(houseShader, skyTexture);
        glassMaterial = std::make_unique<TexturedMaterial>(houseMixedShader, glassTexture);

        asphaltMaterial = std::make_unique<LitMaterial>(houseMixedShader,
                                                        asphaltAlbedo,
                                                        asphaltSpecular,
                                                        asphaltRoughness,
                                                        asphaltEmissive);
    }

    void buildScene() {
        if (!world) {
            return;
        }

        Mesh cubeMesh = Mesh::create_cuboid(glm::vec3(0.0f), glm::vec3(1.0f));
        Mesh glassMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));
        Mesh skySphere = Mesh::create_sphere();
        Mesh sphereMesh = Mesh::create_sphere();
        Mesh asphaltMesh = Mesh::create_plane(glm::vec3(0.0f), glm::vec3(1.0f));

        cubeGpu.upload(cubeMesh);
        glassGpu.upload(glassMesh);
        skyGpu.upload(skySphere);
        sphereGpu.upload(sphereMesh);
        asphaltGpu.upload(asphaltMesh);

        std::string meshPath = std::string(MODELS_DIR) + "/house/house.obj";
        Mesh* loadedMesh = MeshLoader::load(meshPath.c_str());
        if (loadedMesh) {
            houseGpu.upload(*loadedMesh);
        }

        auto& camera = world->get_camera();
        camera.position = glm::vec3(10.f, 5.f, 10.f);
        camera.direction = glm::normalize(glm::vec3(-1.f, 0.f, -1.f));
        camera.up = glm::vec3(0.f, 1.f, 0.f);
        camera.fov = glm::radians(60.0f);
        camera.near = 0.1f;
        camera.far = 100.0f;

        root = world->createEntityWithParams(nullptr);

        player = std::make_unique<Crusader>(*world, root, &cubeGpu, &green, &cubeGpu, &brown);
        player->setPosition({0.0f, 1.0f, 0.0f});
        player->attachCamera(&camera, {0.0f, 2.5f, 6.0f}, {0.0f, 1.0f, 0.0f});

        enemy = CreateEnemy(*world, root, &cubeGpu, &red);
        enemy->setPosition({-4.0f, 1.0f, 0.0f});

        water = spawnRenderable(root, {0.f, 0.f, 0.f}, glm::quat(), {10.f, 1.f, 10.f}, &cubeGpu, &blue);
        island = world->createEntityWithParams(root);

        sand = spawnRenderable(island, {0.f, 0.5f, 0.f}, glm::quat(), {2.f, 1.f, 2.f}, &cubeGpu, &yellow);
        asphalt = spawnRenderable(root, {4.0f, 4.0f, 4.0f}, glm::quat(), {1.0f, 1.0f, 1.0f}, &asphaltGpu, asphaltMaterial.get());
        testhouse = spawnRenderable(root, {10.f, 1.f, 1.f}, glm::quat(), {1.f, 1.f, 1.f}, &houseGpu, houseMixedMaterial.get());
        collisionSphere = spawnRenderable(root, {5.f, 1.0f, -3.f}, glm::quat(), {0.6f, 0.6f, 0.6f}, &sphereGpu, &blue);

        if (player) {
            playerCollider.halfExtents = glm::vec3(0.3f, 0.45f, 0.3f);
        }

        if (loadedMesh) {
            PhysicsCollisionSystem::MeshBounds houseBounds;
            if (PhysicsCollisionSystem::computeMeshBounds(*loadedMesh, houseBounds)) {
                houseCollider.localOffset = houseBounds.center;
                houseCollider.halfExtents = houseBounds.halfExtents;
                houseCollisionReady = true;
            }
        }

        PhysicsCollisionSystem::MeshBounds sphereBounds;
        if (PhysicsCollisionSystem::computeMeshBounds(sphereMesh, sphereBounds)) {
            sphereCollider.localOffset = sphereBounds.center;
            sphereCollider.halfExtents = sphereBounds.halfExtents;
            sphereCollisionReady = true;
        }

        createHouseWindows();
        createTree(canopyOffsets);
    }

    void createHouseWindows() {
        if (!testhouse || !glassMaterial) {
            return;
        }

        spawnRenderable(
            testhouse,
            {2.0f, 1.5f, 0.0f},
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)),
            {2.0f, 1.0f, 1.0f},
            &glassGpu,
            glassMaterial.get()
        );

        spawnRenderable(
            testhouse,
            {-0.2f, 2.0f, -2.75f},
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)),
            {1.0f, 1.0f, 1.5f},
            &glassGpu,
            glassMaterial.get()
        );

        spawnRenderable(
            testhouse,
            {2.0f, 2.0f, -2.75f},
            glm::angleAxis(glm::half_pi<float>() * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f)),
            {1.0f, 1.0f, 1.5f},
            &glassGpu,
            glassMaterial.get()
        );
    }

    void createTree(const std::vector<glm::vec3>& leafOffsets) {
        if (!island) {
            return;
        }

        Entity* tree = world->createEntityWithParams(island);
        spawnRenderable(tree, {0.f, 2.5f, 0.f}, glm::quat(), {0.5f, 5.f, 0.5f}, &cubeGpu, &brown);

        for (size_t i = 0; i < leafOffsets.size(); ++i) {
            glm::quat rotY = glm::angleAxis(1.5f * static_cast<float>(i) + 0.7f, glm::vec3(0.f, 1.f, 0.f));
            glm::quat rotZ = glm::angleAxis(0.7f, glm::vec3(0.f, 0.f, 1.f));

            spawnRenderable(
                tree,
                {leafOffsets[i].x, 6.0f + leafOffsets[i].y, leafOffsets[i].z},
                rotY * rotZ,
                {0.5f, 2.f, 0.5f},
                &cubeGpu,
                &green
            );
        }
    }

    void updatePlayer(float deltaSeconds) {
        if (!player || !window) {
            return;
        }

        PlayerInput input;
        if (Input::isKeyDown(*window, GLFW_KEY_W)) input.move.y -= 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_S)) input.move.y += 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_D)) input.move.x += 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_A)) input.move.x -= 1.0f;
        input.block = Input::isKeyDown(*window, GLFW_KEY_LEFT_SHIFT);
        input.attack = Input::isMouseDown(*window, GLFW_MOUSE_BUTTON_LEFT)
                       || Input::isKeyDown(*window, GLFW_KEY_J);
        input.dodge = Input::isKeyDown(*window, GLFW_KEY_SPACE);

        player->setInput(input);
        player->update(deltaSeconds);
    }

    void updateCollisionSphere(float deltaSeconds) {
        if (!collisionSphere || !window) {
            return;
        }

        glm::vec3 sphereMove(0.0f);
        if (Input::isKeyDown(*window, GLFW_KEY_UP)) sphereMove.z -= 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_DOWN)) sphereMove.z += 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_LEFT)) sphereMove.x -= 1.0f;
        if (Input::isKeyDown(*window, GLFW_KEY_RIGHT)) sphereMove.x += 1.0f;

        float moveLen = glm::length(sphereMove);
        if (moveLen > 0.001f) {
            sphereMove /= moveLen;
            collisionSphere->setPosition(
                collisionSphere->getPosition() + sphereMove * sphereMoveSpeed * deltaSeconds
            );
        }
    }

    void resolveCollisions() {
        if (player && houseCollisionReady && testhouse) {
            PhysicsCollisionSystem::resolveStaticCollision(
                player->entity(),
                playerCollider,
                testhouse,
                houseCollider
            );
        }

        if (collisionSphere && houseCollisionReady && sphereCollisionReady && testhouse) {
            PhysicsCollisionSystem::resolveStaticCollision(
                collisionSphere,
                sphereCollider,
                testhouse,
                houseCollider
            );
        }

        if (player && sphereCollisionReady && collisionSphere) {
            PhysicsCollisionSystem::resolveStaticCollision(
                player->entity(),
                playerCollider,
                collisionSphere,
                sphereCollider
            );
        }
    }

    Entity* spawnRenderable(Entity* parent,
                            const glm::vec3& position,
                            const glm::quat& rotation,
                            const glm::vec3& scale,
                            GpuMesh* mesh,
                            Material* material) {
        if (!world) {
            return nullptr;
        }

        Entity* entity = world->createEntityWithParams(parent, position, rotation, scale);
        auto& renderer = entity->addComponent<MeshRenderer>();
        renderer.mesh = mesh;
        renderer.material = material;
        return entity;
    }

    Window* window = nullptr;
    GLFWwindow* windowHandle = nullptr;
    World* world = nullptr;

    std::shared_ptr<Shader> mainShader;
    std::shared_ptr<Shader> houseShader;
    std::shared_ptr<Shader> houseMixedShader;

    Texture* moonTexture = nullptr;
    Texture* houseTexture = nullptr;
    Texture* glassTexture = nullptr;
    Texture* skyTexture = nullptr;
    Texture* asphaltSpecular = nullptr;
    Texture* asphaltRoughness = nullptr;
    Texture* asphaltEmissive = nullptr;
    Texture* asphaltAlbedo = nullptr;

    TintedMaterial blue;
    TintedMaterial brown;
    TintedMaterial green;
    TintedMaterial yellow;
    TintedMaterial red;

    std::unique_ptr<TexturedMaterial> houseMaterial;
    std::unique_ptr<TexturedMaterial> houseMixedMaterial;
    std::unique_ptr<TexturedMaterial> skyMaterial;
    std::unique_ptr<TexturedMaterial> glassMaterial;
    std::unique_ptr<LitMaterial> asphaltMaterial;

    GpuMesh cubeGpu;
    GpuMesh houseGpu;
    GpuMesh glassGpu;
    GpuMesh skyGpu;
    GpuMesh sphereGpu;
    GpuMesh asphaltGpu;

    Entity* root = nullptr;
    Entity* water = nullptr;
    Entity* island = nullptr;
    Entity* sand = nullptr;
    Entity* asphalt = nullptr;
    Entity* testhouse = nullptr;
    Entity* collisionSphere = nullptr;

    std::unique_ptr<Player> player;
    std::unique_ptr<Enemy> enemy;

    Collider playerCollider;
    Collider houseCollider;
    Collider sphereCollider;
    bool houseCollisionReady = false;
    bool sphereCollisionReady = false;

    float sphereMoveSpeed = 3.0f;
    float lastFpsTime = 0.0f;
    int frameCount = 0;

    bool imguiReady = false;

    const std::vector<glm::vec3> canopyOffsets{
        {0.0f, 0.0f, 0.0f},
        {0.8f, 0.1f, 0.2f},
        {-0.7f, 0.2f, -0.1f},
        {0.2f, 0.3f, -0.8f},
        {-0.3f, -0.1f, 0.7f}
    };
};

int main() {
    ApplicationConfig config;
    config.width = WINDOW_W;
    config.height = WINDOW_H;
    config.title = "Hello World";

    Application app(config);
    GameApp game;
    return app.run(game);
}
