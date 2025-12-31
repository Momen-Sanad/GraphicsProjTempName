#include "ShaderManager.hpp"
#include <iostream>

std::shared_ptr<Shader> ShaderManager::loadShader(
    const std::string& name,
    const std::string& vsPath,
    const std::string& fsPath)
{
    // check cache
    auto it = shaders.find(name);
    if (it != shaders.end())
        return it->second;

    // if not in cache -> load it
    auto shader = std::make_shared<Shader>();
    if (!shader->createFromFiles(vsPath, fsPath)) {
        std::cerr << "Failed to load shader: " << name << std::endl;
        return nullptr;
    }

    // store in cache
    shaders[name] = shader;
    return shader;
}

std::shared_ptr<Shader> ShaderManager::get(const std::string& name) const
{
    auto it = shaders.find(name);
    if (it != shaders.end())
        return it->second;

    return nullptr;
}