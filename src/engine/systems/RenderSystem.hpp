#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class LightSystem;
class Material;
class MeshRenderer;
class SkinnedMaterial;
class SkinnedMeshRenderer;
class World;

namespace engine::ecs {
class Registry;
struct Renderable;
struct SkinnedRenderable;
struct Transform;
}

enum class RenderDebugMode {
    Final = 0,
    Albedo = 1,
    Normals = 2,
    Lighting = 3,
    Bones = 4,
    TextureChannels = 5
};

struct RenderContext {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 viewProj = glm::mat4(1.0f);
    glm::vec3 cameraPosition = glm::vec3(0.0f);
    float deltaTime = 0.0f;
    uint64_t frameIndex = 0;
    RenderDebugMode debugMode = RenderDebugMode::Final;
};

class RenderSystem {
public:
    explicit RenderSystem(LightSystem* lightSystem = nullptr);

    void setLightSystem(LightSystem* lightSystem);
    void setDebugMode(RenderDebugMode mode);
    RenderDebugMode getDebugMode() const { return debugMode_; }

    void render(World& world, const RenderContext& context);
    void render(engine::ecs::Registry& registry, const RenderContext& context);

private:
    void drawStatic(
        const std::shared_ptr<MeshRenderer>& mesh,
        const std::shared_ptr<Material>& material,
        const glm::mat4& model,
        const RenderContext& context);
    void drawSkinned(
        const std::vector<std::shared_ptr<SkinnedMeshRenderer>>& renderers,
        const std::shared_ptr<SkinnedMaterial>& material,
        const glm::mat4& model,
        const RenderContext& context);
    void applyFrameUniforms(class Shader& shader, const RenderContext& context);

    LightSystem* lightSystem_ = nullptr;
    RenderDebugMode debugMode_ = RenderDebugMode::Final;
};
