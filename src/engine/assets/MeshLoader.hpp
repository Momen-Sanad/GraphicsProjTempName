#pragma once
#include "../gl/Mesh.hpp"
#include "ModelData.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

class MeshLoader
{
private:
    static std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_cache;

public:
    static std::shared_ptr<Mesh> readObjMesh(const char* path);
    static std::shared_ptr<ModelData> readGltfModel(const char* path);


    // Memory management
    static void unload(const std::string& filepath);
    static void unload_all();

    // Cache management
    static bool is_loaded(const std::string& filepath);
    static size_t get_loaded_count();
    static std::shared_ptr<Mesh> get_mesh(const std::string& filepath);
};
