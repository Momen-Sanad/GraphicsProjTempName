#pragma once

//   header-only ImGui "Transform Inspector".
// - Edit Position / Rotation (Euler degrees) / Scale
// - Reset button
// - Writes back to the entity only when user edits values
//

#include "imgui.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

// Forward-declare (or include, doesnt really matter)
class Entity;

namespace ImGuiHelpers {

static inline void WritebackVec3IfChanged(Entity* ent, const char* label, glm::vec3& value, glm::vec3 (*getter)(Entity*), void (*setter)(Entity*, const glm::vec3&)) {
    // convenience function (not used below directly; kept for future use)
}

// Main inspector. Safe to call every frame; it will only call entity setters when ImGui reports a change.
static inline void ShowTransformInspector(const char* title, Entity* ent) {
    if (!ent) return;

    ImGui::PushID(title);

    // Read current values from the entity
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    // We call entity getters in a "try" manner using naked forward declarations above.
    // Replace these calls with your actual entity API if the names differ.
    position = ent->getPosition();
    rotation = ent->getRotation();

    // If your entity does not have scale, you can default to (1,1,1)
    try {
        scale = ent->getScale();
    } catch (...) {
        scale = glm::vec3(1.0f);
    }

    // Convert quaternion -> Euler degrees for user-friendly editing
    glm::vec3 eulerDeg = glm::degrees(glm::eulerAngles(rotation));

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Reset button
        if (ImGui::Button("Reset Transform")) {
            position = glm::vec3(0.0f);
            eulerDeg = glm::vec3(0.0f);
            scale = glm::vec3(1.0f);

            ent->setPosition(position);
            ent->setRotation(glm::quat(glm::radians(eulerDeg)));
            ent->setScale(scale);
        }

        ImGui::Separator();

        // Position
        if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.05f)) {
            ent->setPosition(position);
        }

        // Rotation (Euler degrees)
        // Show as degrees to be more intuitive. We convert back to quaternion on change.
        if (ImGui::DragFloat3("Rotation (deg)", glm::value_ptr(eulerDeg), 0.5f)) {
            // convert degrees -> radians then to quat
            glm::vec3 eulerRad = glm::radians(eulerDeg);
            glm::quat q = glm::quat(eulerRad);
            ent->setRotation(q);
        }

        // Scale
        if (ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.01f)) {
            // avoid zero scales
            scale.x = (fabs(scale.x) < 1e-6f) ? 1e-6f : scale.x;
            scale.y = (fabs(scale.y) < 1e-6f) ? 1e-6f : scale.y;
            scale.z = (fabs(scale.z) < 1e-6f) ? 1e-6f : scale.z;
            ent->setScale(scale);
        }

        // nice read-only world matrix viewer
        glm::mat4 worldMat = ent->getWorldMatrix();
        if (ImGui::TreeNode("World Matrix")) {
            for (int r = 0; r < 4; ++r) {
                ImGui::Text("| % .3f % .3f % .3f % .3f |", worldMat[0][r], worldMat[1][r], worldMat[2][r], worldMat[3][r]);
            }
            ImGui::TreePop();
        }
    }

    ImGui::PopID();
}

}

// ----------------------------
// Example usage
//
// #include "imgui_transform_inspector.hpp"
//
// // inside your main loop, once you created a new ImGui frame:
// ImGui::Begin("Inspector");
// ImGuiHelpers::ShowTransformInspector("Selected", island);
// ImGui::End();
//
// ----------------------------