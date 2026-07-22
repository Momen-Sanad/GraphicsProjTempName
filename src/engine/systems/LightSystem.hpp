#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "../components/Light.hpp"
#include <glad/glad.h>

class LightSystem {
public:
    static constexpr int MAX_LIGHTS = 8;
    LightSystem();
    ~LightSystem();
    void shutdownGpuResources();

    // Add a light
    void addLight(const Light& light);
    void clearLights();

    // Initialize the GPU UBO (call once after GL context creation)
    void initUBO();

    // Update the GPU UBO with current light values (call whenever lights change, or every frame)
    void updateUBO();
    bool bindToShader(std::shared_ptr<class Shader> shader);

    // For legacy compatibility - still available if you want to send to individual shader
    void setupLightsInShader(std::shared_ptr<class Shader> shader);

    const std::vector<Light>& getLights() const { return lights; }

private:
    std::vector<Light> lights;
    GLuint ubo = 0;
    const GLuint bindingPoint = 0; // binding point used in shaders (layout(binding = 0) ...)
};
