#pragma once

#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif

#include "../assets/Material.hpp"
#include "../components/MeshRenderer.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>



class Entity {
private:
    Entity* parent = nullptr;
    std::vector<Entity*> children;

    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.f, 0.f, 0.f);
    glm::vec3 scale    = glm::vec3(1.0f);

    MeshRenderer* mesh = nullptr;
    Material* material = nullptr;

public:
    Entity() = default;
    ~Entity();

    // Prevent copying
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // Allow moving
    Entity(Entity&&) = default;
    Entity& operator=(Entity&&) = default;

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
    // Rendering data
    // -------------------------------
    void setMesh(MeshRenderer* m) { mesh = m; }
    void setMaterial(Material* mat) { material = mat; }

    MeshRenderer* getMesh()     const { return mesh; }
    Material*     getMaterial() const { return material; }

    // -------------------------------
    // Transformation
    // -------------------------------
    glm::mat4 getLocalMatrix() const;
    glm::mat4 getWorldMatrix() const;

    // -------------------------------
    // Draw this entity and children
    // -------------------------------
    void draw(const glm::mat4& viewProj);
};