#pragma once
#include "../gl/SkinnedMesh.hpp"
#include "../ecs/AnimationComponent.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>


class MeshLoader
{
private:
    static std::unordered_map<std::string, Mesh*> mesh_cache;     // Cache loaded meshes to avoid reloading

public:
    // Core loading interface
    static Mesh* load_obj(const char* path);
    static ModelData* load_gltf(const char* path);

    // Memory management
    static void unload(const std::string& filepath);
    static void unload_all();

    // Cache management
    static bool is_loaded(const std::string& filepath);
    static size_t get_loaded_count();
    static Mesh* get_mesh(const std::string& filepath);
};