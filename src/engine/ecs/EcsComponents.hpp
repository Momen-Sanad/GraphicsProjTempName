#pragma once

#include "EntityId.hpp"

#include "../animations/Animator.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <string>
#include <vector>

class AnimationClip;
class Material;
class MeshRenderer;
struct ModelAsset;
class Shader;
class SkinnedMaterial;
class SkinnedMeshRenderer;

namespace engine::ecs {

struct Name {
    std::string value;
};

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 worldMatrix = glm::mat4(1.0f);
    bool dirty = true;

    glm::mat4 localMatrix() const {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::mat4_cast(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);
        return t * r * s;
    }
};

struct Hierarchy {
    EntityId parent = InvalidEntity;
    std::vector<EntityId> children;
};

struct Renderable {
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<Material> material;
};

struct SkinnedRenderable {
    std::vector<std::shared_ptr<SkinnedMeshRenderer>> renderers;
    std::shared_ptr<SkinnedMaterial> material;
    std::shared_ptr<ModelAsset> model;
    int skinIndex = -1;
};

struct CameraData {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float fov = glm::radians(60.0f);
    float nearPlane = 0.01f;
    float farPlane = 100.0f;
};

struct LightData {
    int type = 0;
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
    float innerAngle = glm::radians(15.0f);
    float outerAngle = glm::radians(30.0f);
    float intensity = 1.0f;
};

struct AnimatorData {
    std::shared_ptr<ModelAsset> model;
    int skinIndex = -1;
    Animator animator;
    int currentAnimation = -1;
    float speed = 1.0f;
    bool playing = false;
    bool loop = true;
    bool initialized = false;
};

struct BoneAttachment {
    EntityId sourceEntity = InvalidEntity;
    std::string boneName;
    int boneId = -1;
    glm::vec3 localOffset = glm::vec3(0.0f);
    glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 localScale = glm::vec3(1.0f);
};

struct PhysicsBodyData {
    uint32_t bodyId = UINT32_MAX;
};

struct ColliderData {
    glm::vec3 localOffset = glm::vec3(0.0f);
    glm::vec3 halfExtents = glm::vec3(0.5f);
};

} // namespace engine::ecs
