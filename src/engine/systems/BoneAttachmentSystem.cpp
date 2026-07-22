#include "BoneAttachmentSystem.hpp"

#include "TransformSystem.hpp"

#include "../assets/AssetManager.hpp"
#include "../ecs/EcsComponents.hpp"
#include "../ecs/Registry.hpp"

#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cmath>

namespace {
const SkinAsset* resolve_skin(const engine::ecs::AnimatorData& animation)
{
    if (!animation.model) {
        return nullptr;
    }

    const int skinIndex = animation.skinIndex >= 0 ? animation.skinIndex : 0;
    if (skinIndex < 0 || static_cast<size_t>(skinIndex) >= animation.model->skins.size()) {
        return nullptr;
    }

    const SkinAsset& skin = animation.model->skins[static_cast<size_t>(skinIndex)];
    return skin.skeleton ? &skin : nullptr;
}

bool decompose_transform(
    const glm::mat4& matrix,
    glm::vec3& position,
    glm::quat& rotation,
    glm::vec3& scale)
{
    glm::vec3 skew(0.0f);
    glm::vec4 perspective(0.0f);
    if (!glm::decompose(matrix, scale, rotation, position, skew, perspective)) {
        return false;
    }

    rotation = glm::normalize(rotation);
    return true;
}
} // namespace

void BoneAttachmentSystem::update(engine::ecs::Registry& registry, float)
{
    updateAttachments(registry);
}

void BoneAttachmentSystem::updateAttachments(engine::ecs::Registry& registry)
{
    TransformSystem::updateWorldTransforms(registry);

    registry.each<engine::ecs::Transform, engine::ecs::BoneAttachment>(
        [&registry](
            engine::ecs::EntityId target,
            engine::ecs::Transform& targetTransform,
            engine::ecs::BoneAttachment& attachment) {
            if (!registry.isAlive(attachment.sourceEntity)) {
                return;
            }

            auto* sourceTransform = registry.get<engine::ecs::Transform>(attachment.sourceEntity);
            auto* animation = registry.get<engine::ecs::AnimatorData>(attachment.sourceEntity);
            if (!sourceTransform || !animation) {
                return;
            }

            const SkinAsset* skin = resolve_skin(*animation);
            if (!skin || !skin->skeleton) {
                return;
            }

            if (attachment.boneId < 0 && !attachment.boneName.empty()) {
                attachment.boneId = skin->skeleton->get_bone_id(attachment.boneName);
            }
            if (attachment.boneId < 0) {
                return;
            }

            const auto& boneModelMatrices = animation->animator.get_bone_model_matrices();
            if (static_cast<size_t>(attachment.boneId) >= boneModelMatrices.size()) {
                attachment.boneId = -1;
                return;
            }

            glm::mat4 gripTransform(1.0f);
            gripTransform = glm::translate(gripTransform, attachment.localOffset);
            gripTransform *= glm::mat4_cast(attachment.localRotation);
            gripTransform = glm::scale(gripTransform, attachment.localScale);

            const glm::mat4 boneWorld =
                sourceTransform->worldMatrix *
                boneModelMatrices[static_cast<size_t>(attachment.boneId)];
            glm::mat4 targetWorld = boneWorld * gripTransform;

            if (const auto* hierarchy = registry.get<engine::ecs::Hierarchy>(target);
                hierarchy && hierarchy->parent.valid() && registry.isAlive(hierarchy->parent)) {
                if (const auto* parentTransform = registry.get<engine::ecs::Transform>(hierarchy->parent)) {
                    const float determinant = glm::determinant(parentTransform->worldMatrix);
                    if (std::abs(determinant) > 0.000001f) {
                        targetWorld = glm::inverse(parentTransform->worldMatrix) * targetWorld;
                    }
                }
            }

            glm::vec3 position(0.0f);
            glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 scale(1.0f);
            if (!decompose_transform(targetWorld, position, rotation, scale)) {
                return;
            }

            targetTransform.position = position;
            targetTransform.rotation = rotation;
            targetTransform.scale = scale;
            targetTransform.dirty = true;
        });
}
