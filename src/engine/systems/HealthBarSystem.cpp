#include "HealthBarSystem.hpp"

#include "imgui.h"
#include "../components/HealthComponent.hpp"
#include "../components/HurtboxComponent.hpp"
#include "../ecs/Entity.hpp"
#include <glm/gtc/matrix_transform.hpp>

void HealthBarSystem::draw_over_entity(const HealthComponent& health,
                                       const HurtboxComponent& hurtbox,
                                       const Entity& entity,
                                       const glm::mat4& viewProj,
                                       int viewportWidth,
                                       int viewportHeight) {
    if (health.dead || health.maxHP <= 0 || viewportWidth <= 0 || viewportHeight <= 0) {
        return;
    }

    float barHeightOffset = hurtbox.localOffset.y + hurtbox.halfExtents.y + 0.2f;
    glm::vec3 entityPos = glm::vec3(entity.getWorldMatrix()[3]);
    glm::vec3 barWorldPos = entityPos + glm::vec3(0.0f, barHeightOffset, 0.0f);

    glm::vec4 clipPos = viewProj * glm::vec4(barWorldPos, 1.0f);
    if (clipPos.w <= 0.001f) {
        return;
    }

    glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
    if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f) {
        return;
    }

    float screenX = (ndc.x * 0.5f + 0.5f) * static_cast<float>(viewportWidth);
    float screenY = (1.0f - (ndc.y * 0.5f + 0.5f)) * static_cast<float>(viewportHeight);

    float hpFrac = static_cast<float>(health.hp) / static_cast<float>(health.maxHP);
    hpFrac = glm::clamp(hpFrac, 0.0f, 1.0f);

    const float barW = 60.0f;
    const float barH = 6.0f;
    ImVec2 p0(screenX - barW * 0.5f, screenY - barH * 0.5f);
    ImVec2 p1(screenX + barW * 0.5f, screenY + barH * 0.5f);
    ImVec2 fill1(p0.x + (p1.x - p0.x) * hpFrac, p1.y);

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    drawList->AddRectFilled(p0, p1, IM_COL32(0, 0, 0, 180), 2.0f);
    drawList->AddRectFilled(p0, fill1, IM_COL32(220, 60, 60, 220), 2.0f);
    drawList->AddRect(p0, p1, IM_COL32(0, 0, 0, 220), 2.0f);
}
