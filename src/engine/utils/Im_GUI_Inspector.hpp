#pragma once

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../ecs/EcsComponents.hpp"

#include <cmath>

namespace ImGuiHelpers {

static inline void ShowTransformInspector(const char* title, engine::ecs::Transform* transform) {
    if (!transform) {
        return;
    }

    ImGui::PushID(title);

    glm::vec3 position = transform->position;
    glm::quat rotation = transform->rotation;
    glm::vec3 scale = transform->scale;
    glm::vec3 eulerDeg = glm::degrees(glm::eulerAngles(rotation));

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Button("Reset Transform")) {
            position = glm::vec3(0.0f);
            eulerDeg = glm::vec3(0.0f);
            scale = glm::vec3(1.0f);

            transform->position = position;
            transform->rotation = glm::quat(glm::radians(eulerDeg));
            transform->scale = scale;
            transform->dirty = true;
        }

        ImGui::Separator();

        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.05f)) {
            transform->position = position;
            transform->dirty = true;
        }

        if (ImGui::DragFloat3("Rotation (deg)", glm::value_ptr(eulerDeg), 0.5f)) {
            transform->rotation = glm::quat(glm::radians(eulerDeg));
            transform->dirty = true;
        }

        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f)) {
            scale.x = (std::fabs(scale.x) < 1e-6f) ? 1e-6f : scale.x;
            scale.y = (std::fabs(scale.y) < 1e-6f) ? 1e-6f : scale.y;
            scale.z = (std::fabs(scale.z) < 1e-6f) ? 1e-6f : scale.z;
            transform->scale = scale;
            transform->dirty = true;
        }

        if (ImGui::TreeNode("World Matrix")) {
            const glm::mat4& worldMat = transform->worldMatrix;
            for (int r = 0; r < 4; ++r) {
                ImGui::Text("| % .3f % .3f % .3f % .3f |", worldMat[0][r], worldMat[1][r], worldMat[2][r], worldMat[3][r]);
            }
            ImGui::TreePop();
        }
    }

    ImGui::PopID();
}

} // namespace ImGuiHelpers
