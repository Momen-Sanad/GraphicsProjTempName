#ifndef WORLD_HPP
#define WORLD_HPP

#include "EntityId.hpp"
#include "EcsComponents.hpp"
#include "Registry.hpp"
#include "SystemManager.hpp"
#include "../assets/AssetManager.hpp"
#include "../components/Camera.hpp"
#include "../core/FrameScheduler.hpp"
#include "../systems/LightSystem.hpp"
#include "../systems/RenderSystem.hpp"

#include <unordered_set>

enum class DestroyMode {
    Single,
    Recursive
};

class World {
public:
    World();
    ~World() = default;

    engine::ecs::EntityId createEntity(
        const std::string& name = {},
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f));

    engine::ecs::EntityId createRenderable(
        const std::string& name,
        std::shared_ptr<MeshRenderer> mesh,
        std::shared_ptr<Material> material,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f));

    engine::ecs::EntityId createSkinnedRenderable(
        const std::string& name,
        std::vector<std::shared_ptr<SkinnedMeshRenderer>> renderers,
        std::shared_ptr<SkinnedMaterial> material,
        std::shared_ptr<ModelAsset> model,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f));

    bool setParent(engine::ecs::EntityId child, engine::ecs::EntityId parent);
    void destroyEntity(engine::ecs::EntityId entity, DestroyMode mode = DestroyMode::Recursive);
    void clear();
    void shutdownGpuResources();

    engine::ecs::Transform* transform(engine::ecs::EntityId entity);
    const engine::ecs::Transform* transform(engine::ecs::EntityId entity) const;
    
    // --- Accessors ---
    Camera& get_camera() { return camera; }

    engine::ecs::Registry& registry() { return registry_; }
    const engine::ecs::Registry& registry() const { return registry_; }

    AssetManager& assets() { return assetManager_; }
    const AssetManager& assets() const { return assetManager_; }

    FrameScheduler& frameScheduler() { return frameScheduler_; }
    const FrameScheduler& frameScheduler() const { return frameScheduler_; }

    LightSystem& lights() { return lightSystem_; }
    const LightSystem& lights() const { return lightSystem_; }

    RenderSystem& renderer() { return renderSystem_; }
    const RenderSystem& renderer() const { return renderSystem_; }

    engine::ecs::SystemManager& systems() { return systemManager_; }
    const engine::ecs::SystemManager& systems() const { return systemManager_; }
    
private:
    bool wouldCreateHierarchyCycle(engine::ecs::EntityId child, engine::ecs::EntityId parent) const;
    void destroyEntityRecursive(
        engine::ecs::EntityId entity,
        std::unordered_set<engine::ecs::EntityId, engine::ecs::EntityIdHash>& visited);
    void detachFromParent(engine::ecs::EntityId entity);
    void destroyEntitySingle(engine::ecs::EntityId entity);

    engine::ecs::Registry registry_;
    AssetManager assetManager_;
    FrameScheduler frameScheduler_;
    LightSystem lightSystem_;
    RenderSystem renderSystem_;
    engine::ecs::SystemManager systemManager_;
    Camera camera;
};

#endif
