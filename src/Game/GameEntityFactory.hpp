#pragma once

#include "../engine/assets/AssetManager.hpp"
#include "../engine/ecs/World.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <string>
#include <vector>

class SkinnedMaterial;

namespace game {

class GameEntityFactory {
public:
    explicit GameEntityFactory(World& world)
        : world_(world)
    {
    }

    engine::ecs::EntityId createNode(
        const std::string& name,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
    {
        engine::ecs::EntityId entity = world_.createEntity(name, position, rotation, scale);
        world_.setParent(entity, parent);
        return entity;
    }

    engine::ecs::EntityId createStaticRenderable(
        const std::string& name,
        std::shared_ptr<MeshRenderer> mesh,
        std::shared_ptr<Material> material,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
    {
        return world_.createRenderable(name, std::move(mesh), std::move(material), parent, position, rotation, scale);
    }

    engine::ecs::EntityId createSkinnedRenderable(
        const std::string& name,
        std::vector<std::shared_ptr<SkinnedMeshRenderer>> renderers,
        std::shared_ptr<SkinnedMaterial> material,
        std::shared_ptr<ModelAsset> model,
        engine::ecs::EntityId parent = engine::ecs::InvalidEntity,
        const glm::vec3& position = glm::vec3(0.0f),
        const glm::quat& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
    {
        return world_.createSkinnedRenderable(
            name,
            std::move(renderers),
            std::move(material),
            std::move(model),
            parent,
            position,
            rotation,
            scale);
    }

private:
    World& world_;
};

} // namespace game
