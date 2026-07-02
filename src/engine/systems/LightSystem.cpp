#include "LightSystem.hpp"
#include <cstring> // memcpy
#include "../gl/Shader.hpp"
#include <iostream>

// Helper: round up to 16-bytes if we need manual padding (we use vec4 arrays, so fine)
LightSystem::LightSystem() = default;

LightSystem::~LightSystem() {
    if (ubo != 0) {
        glDeleteBuffers(1, &ubo);
    }
}

void LightSystem::addLight(const Light& light) {
    if ((int)lights.size() >= MAX_LIGHTS) {
        std::cerr << "LightSystem::addLight: reached MAX_LIGHTS (" << MAX_LIGHTS << ")\n";
        return;
    }
    lights.push_back(light);
}

void LightSystem::initUBO() {
    if (ubo != 0) return; // already created

    // Layout: we will store data as arrays of vec4 to respect std140:
    // header vec4: x = light_count
    // then arrays: color(vec4: rgb,intensity), pos_type(vec4: xyz,type), dir(vec4: xyz,unused), angles(vec4: innerCos, outerCos, unused, unused)
    const size_t vec4Size = sizeof(glm::vec4);
    const size_t totalVec4 = 1 + 4 * MAX_LIGHTS; // header + (4 vec4 per light)
    const size_t bufferSize = vec4Size * totalVec4;

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightSystem::updateUBO() {
    if (ubo == 0) {
        initUBO();
        if (ubo == 0) {
            return;
        }
    }

    const int count = (int)std::min((size_t)MAX_LIGHTS, lights.size());

    // Build vector<glm::vec4> with layout described above
    std::vector<glm::vec4> data;
    data.reserve(1 + 4 * MAX_LIGHTS);

    // Header vec4: store light_count in x
    glm::vec4 header = glm::vec4(count, 0.0f, 0.0f, 0.0f);
    data.push_back(header);

    // For each light slot (fill unused slots with zeros)
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i < count) {
            const Light& L = lights[i];

            // color.rgb, intensity in w
            glm::vec4 color4(L.color.r, L.color.g, L.color.b, L.intensity);

            // position.xyz, type as float in w
            float typeFloat = static_cast<float>(static_cast<int>(L.type));
            glm::vec4 pos4(L.position.x, L.position.y, L.position.z, typeFloat);

            // direction.xyz, unused w
            glm::vec3 dir = L.direction;
            if (glm::length(dir) > 0.0f) dir = glm::normalize(dir);
            glm::vec4 dir4(dir.x, dir.y, dir.z, 0.0f);

            // angles: cos_inner, cos_outer, unused, unused
            float cosInner = 0.0f, cosOuter = 0.0f;
            if (L.type == LightType::SPOT) {
                cosInner = glm::cos(L.innerAngle);
                cosOuter = glm::cos(L.outerAngle);
            }
            glm::vec4 angles4(cosInner, cosOuter, 0.0f, 0.0f);

            data.push_back(color4);
            data.push_back(pos4);
            data.push_back(dir4);
            data.push_back(angles4);
        } else {
            // push empty slots
            data.push_back(glm::vec4(0.0f));
            data.push_back(glm::vec4(0.0f));
            data.push_back(glm::vec4(0.0f));
            data.push_back(glm::vec4(0.0f));
        }
    }

    // Upload
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4) * data.size(), data.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightSystem::bindToShader(std::shared_ptr<Shader> shader) {
    if (!shader || ubo == 0 || !glad_glGetUniformBlockIndex || !glad_glUniformBlockBinding) {
        return;
    }

    GLuint block_index = glGetUniformBlockIndex(shader->getProgram(), "LightBlock");
    if (block_index != GL_INVALID_INDEX) {
        glUniformBlockBinding(shader->getProgram(), block_index, bindingPoint);
    }
}

void LightSystem::setupLightsInShader(std::shared_ptr<Shader> shader) {
    // Legacy path: keep previous behaviour (expensive) - set individual uniforms if needed.
    // We'll try to populate the same names as before: lights[i].type, lights[i].color, etc and light_count.

    if (!shader) return;
    shader->use();
    bindToShader(shader);
    shader->setUniform("light_count", static_cast<int>(lights.size()));
    shader->setUniform("u_lightCount", static_cast<int>(lights.size()));
    for (size_t i = 0; i < lights.size() && i < (size_t)MAX_LIGHTS; ++i) {
        const Light &L = lights[i];
        std::string idx = std::to_string(i);
        shader->setUniform("lights[" + idx + "].type", static_cast<int>(L.type));
        shader->setUniform("lights[" + idx + "].color", L.color);
        shader->setUniform("lights[" + idx + "].position", L.position);
        shader->setUniform("lights[" + idx + "].direction", L.direction);
        shader->setUniform("lights[" + idx + "].intensity", L.intensity);
        shader->setUniform("lights[" + idx + "].range", 25.0f);

        shader->setUniform("u_lights[" + idx + "].type", static_cast<int>(L.type));
        shader->setUniform("u_lights[" + idx + "].color", L.color);
        shader->setUniform("u_lights[" + idx + "].position", L.position);
        shader->setUniform("u_lights[" + idx + "].direction", L.direction);
        shader->setUniform("u_lights[" + idx + "].intensity", L.intensity);
        shader->setUniform("u_lights[" + idx + "].range", 25.0f);
        if (L.type == LightType::SPOT) {
            shader->setUniform("lights[" + idx + "].cos_inner_angle", glm::cos(L.innerAngle));
            shader->setUniform("lights[" + idx + "].cos_outer_angle", glm::cos(L.outerAngle));
            shader->setUniform("u_lights[" + idx + "].innerCone", glm::cos(L.innerAngle));
            shader->setUniform("u_lights[" + idx + "].outerCone", glm::cos(L.outerAngle));
        }
    }
}
