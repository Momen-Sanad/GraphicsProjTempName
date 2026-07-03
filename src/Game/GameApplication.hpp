#pragma once

#include "GameComponents.hpp"
#include "Entities/Player.hpp"

#include "../engine/assets/AssetManager.hpp"
#include "../engine/assets/LitMaterial.hpp"
#include "../engine/assets/SkinnedMaterial.hpp"
#include "../engine/assets/TexturedMaterial.hpp"
#include "../engine/assets/TintedMaterial.hpp"
#include "../engine/ecs/World.hpp"
#include "../engine/systems/AnimationSystem.hpp"
#include "../engine/systems/RenderSystem.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>
#include <vector>

class Window;

namespace game {

struct GameAssets {
    std::shared_ptr<Shader> mainShader;
    std::shared_ptr<Shader> houseShader;
    std::shared_ptr<Shader> blendedShader;
    std::shared_ptr<Shader> lightShader;
    std::shared_ptr<Shader> skinnedShader;

    std::shared_ptr<TintedMaterial> blue;
    std::shared_ptr<TintedMaterial> brown;
    std::shared_ptr<TintedMaterial> green;
    std::shared_ptr<TintedMaterial> yellow;
    std::shared_ptr<TintedMaterial> red;
    std::shared_ptr<TintedMaterial> xpGold;
    std::shared_ptr<TexturedMaterial> house;
    std::shared_ptr<TexturedMaterial> houseMixed;
    std::shared_ptr<TexturedMaterial> glass;
    std::shared_ptr<LitMaterial> asphalt;

    std::shared_ptr<Mesh> houseMesh;
    std::shared_ptr<MeshRenderer> cubeRenderer;
    std::shared_ptr<MeshRenderer> houseRenderer;
    std::shared_ptr<MeshRenderer> glassRenderer;
    std::shared_ptr<MeshRenderer> sphereRenderer;
    std::shared_ptr<MeshRenderer> planeRenderer;
    std::shared_ptr<MeshRenderer> xpOrbRenderer;

    std::shared_ptr<ModelAsset> swordmanModel;
    std::shared_ptr<ModelAsset> skeletonModel;
    std::vector<std::shared_ptr<SkinnedMeshRenderer>> swordmanRenderers;
    std::vector<std::shared_ptr<SkinnedMeshRenderer>> skeletonRenderers;
    int swordmanBaseAnimation = -1;
    int swordmanAttackAnimation = -1;
};

class GameApplication {
public:
    ~GameApplication();

    int run();

private:
    bool loadAssets();
    void setupWorld();
    void setupLights();
    void spawnInitialWave();
    engine::ecs::EntityId spawnEnemy(const glm::vec3& position);
    void spawnXpBurst(const glm::vec3& position);

    void update(Window& window, float deltaTime);
    void updateCombat(float deltaTime);
    void updateWaves();
    void updateXpOrbs(float deltaTime);
    void updateCollisions();
    void updateCharacterLights();
    void updatePlayerAnimationState();

    void render(Window& window, float deltaTime, uint64_t frameIndex);
    void renderUi(const glm::mat4& viewProjection, int width, int height);
    void renderOverlayBars();
    void renderUpgradeMenu();

    std::shared_ptr<SkinnedMaterial> makeSkinnedMaterial(const std::shared_ptr<ModelAsset>& model) const;

    World world_;
    AnimationSystem animationSystem_;
    GameAssets assets_;
    GameplayState state_;
    std::unique_ptr<Player> player_;
    RenderDebugMode renderDebugMode_ = RenderDebugMode::Final;
    bool playerAttackAnimationActive_ = false;
};

} // namespace game
