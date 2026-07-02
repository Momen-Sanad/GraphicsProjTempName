#include "World.hpp"

#include <algorithm>
#include <unordered_set>

// ------------------------------------------------------------
// Constructor for World class (default constructor)
// ------------------------------------------------------------
World::World()
    : renderSystem_(&lightSystem_)
{
}

engine::ecs::EntityId World::createEntity(
    const std::string& name,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale)
{
    engine::ecs::EntityId entity = registry_.createEntity();
    auto& transform = registry_.emplace<engine::ecs::Transform>(entity);
    transform.position = position;
    transform.rotation = rotation;
    transform.scale = scale;
    transform.dirty = true;
    registry_.emplace<engine::ecs::Hierarchy>(entity);
    if (!name.empty()) {
        registry_.emplace<engine::ecs::Name>(entity, engine::ecs::Name{name});
    }
    return entity;
}

engine::ecs::EntityId World::createRenderable(
    const std::string& name,
    std::shared_ptr<MeshRenderer> mesh,
    std::shared_ptr<Material> material,
    engine::ecs::EntityId parent,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale)
{
    engine::ecs::EntityId entity = createEntity(name, position, rotation, scale);
    registry_.emplace<engine::ecs::Renderable>(
        entity,
        engine::ecs::Renderable{std::move(mesh), std::move(material)});
    setParent(entity, parent);
    return entity;
}

engine::ecs::EntityId World::createSkinnedRenderable(
    const std::string& name,
    std::vector<std::shared_ptr<SkinnedMeshRenderer>> renderers,
    std::shared_ptr<SkinnedMaterial> material,
    std::shared_ptr<ModelAsset> model,
    engine::ecs::EntityId parent,
    const glm::vec3& position,
    const glm::quat& rotation,
    const glm::vec3& scale)
{
    engine::ecs::EntityId entity = createEntity(name, position, rotation, scale);
    const int skinIndex = model && !model->skins.empty() ? 0 : -1;
    registry_.emplace<engine::ecs::SkinnedRenderable>(
        entity,
        engine::ecs::SkinnedRenderable{
            std::move(renderers),
            std::move(material),
            std::move(model),
            skinIndex});
    setParent(entity, parent);
    return entity;
}

bool World::setParent(engine::ecs::EntityId child, engine::ecs::EntityId parent) {
    if (!registry_.isAlive(child)) {
        return false;
    }

    if (parent.valid()) {
        if (!registry_.isAlive(parent) || parent == child || wouldCreateHierarchyCycle(child, parent)) {
            return false;
        }
    }

    detachFromParent(child);
    auto& hierarchy = registry_.ensure<engine::ecs::Hierarchy>(child);
    hierarchy.parent = engine::ecs::InvalidEntity;

    if (parent.valid()) {
        hierarchy.parent = parent;
        auto& parentHierarchy = registry_.ensure<engine::ecs::Hierarchy>(parent);
        if (std::find(parentHierarchy.children.begin(), parentHierarchy.children.end(), child) ==
            parentHierarchy.children.end()) {
            parentHierarchy.children.push_back(child);
        }
    }

    return true;
}

void World::destroyEntity(engine::ecs::EntityId entity, DestroyMode mode) {
    if (!registry_.isAlive(entity)) {
        return;
    }

    if (mode == DestroyMode::Recursive) {
        std::unordered_set<engine::ecs::EntityId, engine::ecs::EntityIdHash> visited;
        destroyEntityRecursive(entity, visited);
        return;
    } else if (auto* hierarchy = registry_.get<engine::ecs::Hierarchy>(entity)) {
        for (engine::ecs::EntityId child : hierarchy->children) {
            if (auto* childHierarchy = registry_.get<engine::ecs::Hierarchy>(child)) {
                childHierarchy->parent = engine::ecs::InvalidEntity;
            }
        }
        hierarchy->children.clear();
    }

    destroyEntitySingle(entity);
}

void World::clear() {
    registry_.clear();
}

void World::shutdownGpuResources() {
    registry_.clear();
    systemManager_.clear();
    assetManager_.clear();
    lightSystem_.shutdownGpuResources();
}

engine::ecs::Transform* World::transform(engine::ecs::EntityId entity) {
    return registry_.get<engine::ecs::Transform>(entity);
}

const engine::ecs::Transform* World::transform(engine::ecs::EntityId entity) const {
    return registry_.get<engine::ecs::Transform>(entity);
}

bool World::wouldCreateHierarchyCycle(engine::ecs::EntityId child, engine::ecs::EntityId parent) const {
    engine::ecs::EntityId current = parent;
    while (registry_.isAlive(current)) {
        if (current == child) {
            return true;
        }

        const auto* hierarchy = registry_.get<engine::ecs::Hierarchy>(current);
        if (!hierarchy || !hierarchy->parent.valid()) {
            return false;
        }
        current = hierarchy->parent;
    }
    return false;
}

void World::destroyEntityRecursive(
    engine::ecs::EntityId entity,
    std::unordered_set<engine::ecs::EntityId, engine::ecs::EntityIdHash>& visited)
{
    if (!registry_.isAlive(entity) || !visited.insert(entity).second) {
        return;
    }

    std::vector<engine::ecs::EntityId> children;
    if (const auto* hierarchy = registry_.get<engine::ecs::Hierarchy>(entity)) {
        children = hierarchy->children;
    }

    for (engine::ecs::EntityId child : children) {
        destroyEntityRecursive(child, visited);
    }

    if (auto* hierarchy = registry_.get<engine::ecs::Hierarchy>(entity)) {
        hierarchy->children.clear();
    }
    destroyEntitySingle(entity);
}

void World::detachFromParent(engine::ecs::EntityId entity) {
    auto* hierarchy = registry_.get<engine::ecs::Hierarchy>(entity);
    if (!hierarchy || !registry_.isAlive(hierarchy->parent)) {
        return;
    }

    if (auto* parentHierarchy = registry_.get<engine::ecs::Hierarchy>(hierarchy->parent)) {
        auto& siblings = parentHierarchy->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), entity), siblings.end());
    }

    hierarchy->parent = engine::ecs::InvalidEntity;
}

void World::destroyEntitySingle(engine::ecs::EntityId entity) {
    detachFromParent(entity);
    registry_.destroyEntity(entity);
}
