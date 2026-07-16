#pragma once

#include "../engine/assets/Material.hpp"
#include "../engine/animations/Skeleton.hpp"
#include "../engine/components/MeshRenderer.hpp"
#include "../engine/ecs/EcsComponents.hpp"
#include "../engine/ecs/World.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace game {

struct SwordVisualAssets {
    std::shared_ptr<MeshRenderer> bladeRenderer;
    std::shared_ptr<MeshRenderer> guardRenderer;
    std::shared_ptr<MeshRenderer> gripRenderer;
    std::shared_ptr<MeshRenderer> pommelRenderer;
    std::shared_ptr<Material> bladeMaterial;
    std::shared_ptr<Material> guardMaterial;
    std::shared_ptr<Material> gripMaterial;
};

inline constexpr const char* kSwordmanHandLocatorBone = "R_Hand_locator_055";
inline constexpr const char* kSwordmanRightHandBone = "UnknownSoldier_RightHand_034";

inline std::string resolvePlayerSwordHandBoneName(const Skeleton& skeleton)
{
    if (skeleton.get_bone_id(kSwordmanHandLocatorBone) >= 0) {
        return kSwordmanHandLocatorBone;
    }
    if (skeleton.get_bone_id(kSwordmanRightHandBone) >= 0) {
        return kSwordmanRightHandBone;
    }
    return {};
}

inline glm::quat playerSwordGripRotation()
{
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

inline glm::vec3 playerSwordGripOffset()
{
    return glm::vec3(0.0f, -0.02f, 0.02f);
}

inline glm::vec3 playerSwordGripScale()
{
    return glm::vec3(100.0f);
}

inline glm::vec3 playerSwordGripScaleForBone(const Skeleton& skeleton, int boneId)
{
    glm::vec3 scale = playerSwordGripScale();
    if (boneId < 0 || boneId >= skeleton.get_bone_count()) {
        return scale;
    }

    glm::vec3 boneScale(1.0f);
    glm::quat boneRotation(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 bonePosition(0.0f);
    glm::vec3 skew(0.0f);
    glm::vec4 perspective(0.0f);
    if (!glm::decompose(skeleton.get_bone(boneId).local_transform, boneScale, boneRotation, bonePosition, skew, perspective)) {
        return scale;
    }

    for (int axis = 0; axis < 3; ++axis) {
        if (std::abs(boneScale[axis]) > 0.0001f) {
            scale[axis] /= std::abs(boneScale[axis]);
        }
    }
    return scale;
}

inline glm::mat4 playerSwordBoneBindModelMatrix(const Skeleton& skeleton, int boneId)
{
    if (boneId < 0 || boneId >= skeleton.get_bone_count()) {
        return glm::mat4(1.0f);
    }

    std::vector<int> chain;
    chain.reserve(static_cast<size_t>(skeleton.get_bone_count()));
    int current = boneId;
    for (int guard = 0; current >= 0 && guard < skeleton.get_bone_count(); ++guard) {
        chain.push_back(current);
        current = skeleton.get_bone(current).parent_id;
    }

    glm::mat4 model(1.0f);
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        model *= skeleton.get_bone(*it).local_transform;
    }
    return model;
}

inline glm::quat rotationBetweenDirections(glm::vec3 from, glm::vec3 to)
{
    constexpr float epsilon = 0.0001f;
    if (glm::length(from) <= epsilon || glm::length(to) <= epsilon) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    from = glm::normalize(from);
    to = glm::normalize(to);
    const float cosTheta = glm::clamp(glm::dot(from, to), -1.0f, 1.0f);
    if (cosTheta > 1.0f - epsilon) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    if (cosTheta < -1.0f + epsilon) {
        glm::vec3 axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), from);
        if (glm::length(axis) <= epsilon) {
            axis = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), from);
        }
        return glm::angleAxis(glm::pi<float>(), glm::normalize(axis));
    }

    const glm::vec3 axis = glm::cross(from, to);
    const float s = std::sqrt((1.0f + cosTheta) * 2.0f);
    const float invS = 1.0f / s;
    return glm::normalize(glm::quat(
        s * 0.5f,
        axis.x * invS,
        axis.y * invS,
        axis.z * invS));
}

inline glm::quat playerSwordGripRotationForBone(const Skeleton& skeleton, int boneId)
{
    if (boneId < 0 || boneId >= skeleton.get_bone_count()) {
        return playerSwordGripRotation();
    }

    const glm::mat4 boneModel = playerSwordBoneBindModelMatrix(skeleton, boneId);
    const glm::mat3 boneBasis(boneModel);
    if (std::abs(glm::determinant(boneBasis)) <= 0.000001f) {
        return playerSwordGripRotation();
    }

    const glm::vec3 desiredModelForward(0.0f, 0.0f, 1.0f);
    const glm::vec3 desiredBoneLocalForward =
        glm::normalize(glm::inverse(boneBasis) * desiredModelForward);
    return rotationBetweenDirections(glm::vec3(0.0f, 0.0f, 1.0f), desiredBoneLocalForward);
}

inline engine::ecs::BoneAttachment makePlayerSwordAttachment(
    engine::ecs::EntityId sourceEntity,
    const Skeleton& skeleton)
{
    engine::ecs::BoneAttachment attachment;
    attachment.sourceEntity = sourceEntity;
    attachment.boneName = resolvePlayerSwordHandBoneName(skeleton);
    attachment.boneId = attachment.boneName.empty() ? -1 : skeleton.get_bone_id(attachment.boneName);
    attachment.localOffset = playerSwordGripOffset();
    attachment.localRotation = playerSwordGripRotationForBone(skeleton, attachment.boneId);
    attachment.localScale = playerSwordGripScaleForBone(skeleton, attachment.boneId);
    return attachment;
}

inline engine::ecs::EntityId createPlayerSword(
    World& world,
    engine::ecs::EntityId parent,
    const SwordVisualAssets& assets)
{
    engine::ecs::EntityId sword = world.createEntity("SwordRoot");
    world.setParent(sword, parent);

    auto createPart = [&world, sword](
        const std::string& name,
        const std::shared_ptr<MeshRenderer>& renderer,
        const std::shared_ptr<Material>& material) {
        if (!renderer || !material) {
            return engine::ecs::InvalidEntity;
        }
        return world.createRenderable(
            name,
            renderer,
            material,
            sword,
            glm::vec3(0.0f),
            glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f));
    };

    createPart("SwordBlade", assets.bladeRenderer, assets.bladeMaterial);
    createPart("SwordGuard", assets.guardRenderer, assets.guardMaterial);
    createPart("SwordGrip", assets.gripRenderer, assets.gripMaterial);
    createPart("SwordPommel", assets.pommelRenderer, assets.guardMaterial);

    return sword;
}

} // namespace game
