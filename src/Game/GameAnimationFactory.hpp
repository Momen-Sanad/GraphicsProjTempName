#pragma once

#include "../engine/assets/AssetManager.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <array>
#include <initializer_list>
#include <memory>
#include <string_view>

namespace game {

namespace animation_factory_detail {

struct BoneSample {
    glm::vec3 position{0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
};

struct AttackBones {
    int spine = -1;
    int chest = -1;
    int rightShoulder = -1;
    int rightArm = -1;
    int rightForeArm = -1;
    int rightHand = -1;
    int leftShoulder = -1;
    int leftArm = -1;
    int leftForeArm = -1;
};

inline int findBone(const Skeleton& skeleton, std::initializer_list<std::string_view> names)
{
    for (std::string_view name : names) {
        const int id = skeleton.get_bone_id(std::string(name));
        if (id >= 0) {
            return id;
        }
    }
    return -1;
}

inline int findAnimation(const ModelAsset& model, std::string_view name)
{
    for (size_t index = 0; index < model.animations.size(); ++index) {
        const auto& clip = model.animations[index];
        if (clip && clip->get_name() == name) {
            return static_cast<int>(index);
        }
    }
    return -1;
}

inline BoneSample sampleBindPose(const Skeleton& skeleton, int boneId)
{
    BoneSample sample;
    if (boneId < 0 || boneId >= skeleton.get_bone_count()) {
        return sample;
    }

    glm::vec3 skew(0.0f);
    glm::vec4 perspective(0.0f);
    glm::decompose(
        skeleton.get_bone(boneId).local_transform,
        sample.scale,
        sample.rotation,
        sample.position,
        skew,
        perspective);
    sample.rotation = glm::normalize(sample.rotation);
    return sample;
}

inline BoneSample sampleBase(const Skeleton& skeleton, const AnimationClip* baseClip, int boneId, float time)
{
    BoneSample sample = sampleBindPose(skeleton, boneId);
    if (!baseClip) {
        return sample;
    }

    const BoneAnimation* baseBone = baseClip->get_bone_animation(boneId);
    if (!baseBone) {
        return sample;
    }

    const float baseTime = baseClip->get_duration() > 0.0f
        ? glm::clamp(time, 0.0f, baseClip->get_duration())
        : 0.0f;
    if (!baseBone->position_keys.empty()) {
        sample.position = baseBone->get_position(baseTime);
    }
    if (!baseBone->rotation_keys.empty()) {
        sample.rotation = baseBone->get_rotation(baseTime);
    }
    if (!baseBone->scale_keys.empty()) {
        sample.scale = baseBone->get_scale(baseTime);
    }
    return sample;
}

inline glm::quat eulerDegrees(float x, float y, float z)
{
    return glm::quat(glm::radians(glm::vec3(x, y, z)));
}

inline glm::quat swordForwardAttackTwist(bool windup)
{
    return eulerDegrees(0.0f, windup ? 45.0f : 80.0f, 0.0f);
}

inline glm::quat attackDelta(const AttackBones& bones, int boneId, size_t phase)
{
    if (phase == 0 || phase == 3) {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    }

    const bool windup = phase == 1;
    if (boneId == bones.spine) {
        return windup ? eulerDegrees(0.0f, -8.0f, -3.0f) : eulerDegrees(0.0f, 14.0f, 5.0f);
    }
    if (boneId == bones.chest) {
        return windup ? eulerDegrees(-4.0f, -14.0f, -6.0f) : eulerDegrees(8.0f, 24.0f, 8.0f);
    }
    if (boneId == bones.rightShoulder) {
        return windup ? eulerDegrees(-8.0f, 0.0f, -24.0f) : eulerDegrees(12.0f, 0.0f, 30.0f);
    }
    if (boneId == bones.rightArm) {
        return windup ? eulerDegrees(-58.0f, 22.0f, -32.0f) : eulerDegrees(52.0f, -44.0f, 42.0f);
    }
    if (boneId == bones.rightForeArm) {
        return windup ? eulerDegrees(-36.0f, 5.0f, -12.0f) : eulerDegrees(34.0f, -16.0f, 22.0f);
    }
    if (boneId == bones.rightHand) {
        const glm::quat handPose =
            windup ? eulerDegrees(0.0f, -12.0f, -26.0f) : eulerDegrees(0.0f, 26.0f, 58.0f);
        return glm::normalize(handPose * swordForwardAttackTwist(windup));
    }
    if (boneId == bones.leftShoulder) {
        return windup ? eulerDegrees(4.0f, 0.0f, 12.0f) : eulerDegrees(-6.0f, 0.0f, -16.0f);
    }
    if (boneId == bones.leftArm) {
        return windup ? eulerDegrees(18.0f, -8.0f, 16.0f) : eulerDegrees(-18.0f, 12.0f, -20.0f);
    }
    if (boneId == bones.leftForeArm) {
        return windup ? eulerDegrees(12.0f, 0.0f, 8.0f) : eulerDegrees(-12.0f, 0.0f, -10.0f);
    }
    return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
}

inline AttackBones findSwordmanAttackBones(const Skeleton& skeleton)
{
    AttackBones bones;
    bones.spine = findBone(skeleton, {"UnknownSoldier_Spine1_03", "mixamorig:Spine1_20"});
    bones.chest = findBone(skeleton, {"UnknownSoldier_Spine2_04", "mixamorig:Spine2_19"});
    bones.rightShoulder = findBone(skeleton, {"UnknownSoldier_RightShoulder_031", "mixamorig:RightShoulder_3"});
    bones.rightArm = findBone(skeleton, {"UnknownSoldier_RightArm_032", "mixamorig:RightArm_4"});
    bones.rightForeArm = findBone(skeleton, {"UnknownSoldier_RightForeArm_033", "mixamorig:RightForeArm_5"});
    bones.rightHand = findBone(skeleton, {"UnknownSoldier_RightHand_034", "mixamorig:RightHand_6"});
    bones.leftShoulder = findBone(skeleton, {"UnknownSoldier_LeftShoulder_08", "mixamorig:LeftShoulder_17"});
    bones.leftArm = findBone(skeleton, {"UnknownSoldier_LeftArm_09", "mixamorig:LeftArm_18"});
    bones.leftForeArm = findBone(skeleton, {"UnknownSoldier_LeftForeArm_010", "mixamorig:LeftForeArm_19"});
    return bones;
}

} // namespace animation_factory_detail

inline int ensurePlayerAttackAnimation(ModelAsset& model)
{
    constexpr std::string_view kAttackName = "player_sword_attack";
    if (const int existing = animation_factory_detail::findAnimation(model, kAttackName); existing >= 0) {
        return existing;
    }

    if (model.skins.empty() || !model.skins.front().skeleton) {
        return -1;
    }

    const Skeleton& skeleton = *model.skins.front().skeleton;
    if (skeleton.get_bone_count() <= 0) {
        return -1;
    }

    const auto bones = animation_factory_detail::findSwordmanAttackBones(skeleton);
    if (bones.rightArm < 0 && bones.rightForeArm < 0 && bones.rightHand < 0) {
        return -1;
    }

    const AnimationClip* baseClip = model.animations.empty() ? nullptr : model.animations.front().get();
    constexpr std::array<float, 4> keyTimes = {0.0f, 0.07f, 0.16f, 0.25f};
    auto clip = std::make_shared<AnimationClip>(std::string(kAttackName), keyTimes.back(), 1.0f);

    for (int boneId = 0; boneId < skeleton.get_bone_count(); ++boneId) {
        BoneAnimation boneAnimation;
        boneAnimation.bone_id = boneId;

        for (size_t phase = 0; phase < keyTimes.size(); ++phase) {
            const float time = keyTimes[phase];
            const auto sample = animation_factory_detail::sampleBase(skeleton, baseClip, boneId, time);
            const glm::quat delta = animation_factory_detail::attackDelta(bones, boneId, phase);

            boneAnimation.position_keys.push_back({time, sample.position});
            boneAnimation.rotation_keys.push_back({time, glm::normalize(sample.rotation * delta)});
            boneAnimation.scale_keys.push_back({time, sample.scale});
        }

        clip->add_bone_animation(boneAnimation);
    }

    model.animations.push_back(std::move(clip));
    return static_cast<int>(model.animations.size() - 1);
}

} // namespace game
