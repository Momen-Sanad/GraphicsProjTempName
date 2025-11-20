#pragma once
#include "../ecs/Component.hpp"
#include <glad/gl.h>

class MeshRenderer : public Component {
public:
    void initialize(Entity* entity) override;
    void update(float dt) override;

    GLuint get_vao() const;
    GLuint get_texture() const;
    GLuint get_element_count() const;

private:
    GLuint vao, texture, element_count;
};