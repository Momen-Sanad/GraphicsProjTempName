#pragma once

#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/gl.h>
#endif

#include "../assets/Material.hpp"
#include "../components/MeshRenderer.hpp"
#include "../components/SkinnedMeshRenderer.hpp"
#include "../assets/SkinnedMaterial.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include "Component.hpp"

// Forward declarations
struct ModelData;
class AnimationComponent;

class Entity {
private:
    Entity* parent = nullptr;
    std::vector<Entity*> children;

    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.f, 0.f, 0.f);
    glm::vec3 scale    = glm::vec3(1.0f);

    // Basic mesh rendering (backward compatible)
    MeshRenderer* mesh = nullptr;
    Material* material = nullptr;

    // Skinned mesh rendering (new functionality)
    std::vector<SkinnedMeshRenderer*> skinned_renderers;
    SkinnedMaterial* skinned_material = nullptr;
    std::shared_ptr<ModelData> model_data;

    // Entity name for identification
    std::string name;

    std::vector<Component*> components;

public:
    Entity() = default;
    explicit Entity(const std::string& name) : name(name) {}
    ~Entity();

    // Prevent copying
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Allow moving
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

    // -------------------------------
    // Name accessors
    // -------------------------------
    void setName(const std::string& n) { name = n; }
    const std::string& getName() const { return name; }

    // -------------------------------
    // Transform accessors
    // -------------------------------
    void setPosition(const glm::vec3& p)  { position = p; }
    void setRotation(const glm::quat& q)  { rotation = q; }
    void setScale(const glm::vec3& s)     { scale = s; }

    glm::vec3 getPosition() const { return position; }
    glm::quat getRotation() const { return rotation; }
    glm::vec3 getScale()    const { return scale; }

    // convenience: apply an incremental rotation (delta quaternion)
    void rotateBy(const glm::quat& dq) { rotation = dq * rotation; }

    // -------------------------------
    // Hierarchy management
    // -------------------------------
    void setParent(Entity* newParent);
    Entity* getParent() const { return parent; }

    const std::vector<Entity*>& getChildren() const { return children; }

    // -------------------------------
    // Basic Rendering data (backward compatible)
    // -------------------------------
    void setMesh(MeshRenderer* m) { mesh = m; }
    void setMaterial(Material* mat) { material = mat; }

    MeshRenderer* getMesh()     const { return mesh; }
    Material*     getMaterial() const { return material; }

    // -------------------------------
    // Skinned/Animated Model Support
    // -------------------------------
    
    // Set model data (includes skeleton, animations, meshes)
    void setModelData(std::shared_ptr<ModelData> data);
    std::shared_ptr<ModelData> getModelData() const { return model_data; }
    bool hasModelData() const { return model_data != nullptr; }

    // Skinned material for animated models
    void setSkinnedMaterial(SkinnedMaterial* mat) { skinned_material = mat; }
    SkinnedMaterial* getSkinnedMaterial() const { return skinned_material; }

    // Add/get skinned mesh renderers
    void addSkinnedRenderer(SkinnedMeshRenderer* renderer);
    void clearSkinnedRenderers();
    const std::vector<SkinnedMeshRenderer*>& getSkinnedRenderers() const { return skinned_renderers; }
    size_t getSkinnedRendererCount() const { return skinned_renderers.size(); }

    // Check if entity has skinned/animated rendering
    bool hasSkinnedRendering() const { 
        return !skinned_renderers.empty() && skinned_material != nullptr; 
    }

    // Convenience method to setup skinned model from ModelData
    // Creates renderers and uploads mesh data to GPU
    void setupSkinnedModel(std::shared_ptr<ModelData> data, SkinnedMaterial* mat);

    // Get the AnimationComponent if attached
    AnimationComponent* getAnimationComponent();
    const AnimationComponent* getAnimationComponent() const;

    // -------------------------------
    // Component management
    // -------------------------------
    void addComponent(Component* component);
    void removeComponent(Component* component);
    
    template<typename T>
    T* getComponent() {
        for (Component* c : components) {
            T* casted = dynamic_cast<T*>(c);
            if (casted) return casted;
        }
        return nullptr;
    }

    template<typename T>
    const T* getComponent() const {
        for (const Component* c : components) {
            const T* casted = dynamic_cast<const T*>(c);
            if (casted) return casted;
        }
        return nullptr;
    }

    const std::vector<Component*>& getComponents() const { return components; }

    // Update all components attached to this entity and children
    void updateComponents(float deltaTime);

    // -------------------------------
    // Transformation
    // -------------------------------
    glm::mat4 getLocalMatrix() const;
    glm::mat4 getWorldMatrix() const;

    // -------------------------------
    // Draw this entity and children
    // -------------------------------
    void draw(const glm::mat4& viewProj);

    // Add this method to the Entity class public section

// Sets all skinned mesh renderers at once (replaces current list)
    void setSkinnedRenderers(const std::vector<SkinnedMeshRenderer*>& renderers) {
        skinned_renderers = renderers;
    }

    // Draw skinned meshes (called internally or can be called externally)
    void drawSkinned(const glm::mat4& viewProj);
};