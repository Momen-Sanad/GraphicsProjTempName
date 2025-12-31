#pragma once
#include <glad/glad.h>
#include "Material.hpp"
#include <glm/glm.hpp>

class TintedMaterial : public Material {
public:
    glm::vec4 tint = glm::vec4(1.0f);

    TintedMaterial() = default;

    void setup() override {
        if (!shader) {
            glUseProgram(0);
            return;
        }

        shader->use();
        GLint loc = getUniformLocation("tint");
        if (loc != -1)
            glUniform4f(loc, tint.r, tint.g, tint.b, tint.a);
    }
};