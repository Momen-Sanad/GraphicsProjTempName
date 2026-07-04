#pragma once

#include "../ecs/System.hpp"

namespace engine::ecs {
class Registry;
}

class BoneAttachmentSystem final : public engine::ecs::System {
public:
    void update(engine::ecs::Registry& registry, float deltaTime) override;

    static void updateAttachments(engine::ecs::Registry& registry);
};
