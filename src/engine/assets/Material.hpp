#pragma once
#include <glad/glad.h>
#include "../gl/Shader.hpp"
#include <memory>

class Material {
protected:
    std::shared_ptr<Shader> shader;

public:
    Material() = default;
    virtual ~Material() = default;

    // Prevent copying
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    // Allow move
    Material(Material&&) = default;
    Material& operator=(Material&&) = default;

    void setShader(std::shared_ptr<Shader> s) { shader = std::move(s); }
    std::shared_ptr<Shader> getShader() const { return shader; }

    virtual void setup() = 0;
    
    GLint getUniform(const std::string& name) const { 
        return getUniformLocation(name); 
    }
    
protected:
    GLint getUniformLocation(const std::string& name) const {
        if (!shader) return -1;
        return shader->getUniformLocation(name);
    }
};