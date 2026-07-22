#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../gl/Shader.hpp"

class ShaderManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

public:
    ShaderManager() = default;

    // Loads and caches a shader
    std::shared_ptr<Shader> loadShader(const std::string& name,
                                       const std::string& vsPath,
                                       const std::string& fsPath);

    // Retrieve a loaded shader
    std::shared_ptr<Shader> get(const std::string& name) const;
};
