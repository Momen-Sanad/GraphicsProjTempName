#include "MeshLoader.hpp"
#include <iostream>
// potential file to load for animations + kf + bone locations: file.dae
// consider open source lib for fpx
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh* MeshLoader::get_mesh(const std::string& filepath)
{
    auto it = mesh_cache.find(filepath);

    if (it != mesh_cache.end())
        return it->second;
    else
    {
        std::cout << "No registered mesh with filepath: '" << filepath << "'" << std::endl;
        return nullptr;
    }
}

size_t hash_combine(size_t seed, size_t h) {
    return seed ^ (h + 0x9e3779b9) + (seed << 6) + (seed >> 2);
}

struct VertexHash
{
    size_t operator()(const Vertex& v) const
    {
        size_t seed = 0;
        seed = hash_combine(seed, std::hash<float>()(v.position.x));
        seed = hash_combine(seed, std::hash<float>()(v.position.y));
        seed = hash_combine(seed, std::hash<float>()(v.position.z));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.r));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.g));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.b));
        seed = hash_combine(seed, std::hash<uint8_t>()(v.color.a));
        seed = hash_combine(seed, std::hash<float>()(v.tex_coord.s));
        seed = hash_combine(seed, std::hash<float>()(v.tex_coord.t));
        return seed;
    }
};


// supports loading a mesh from an .obj file
Mesh* MeshLoader::load(const char* path)
{
    // Check file extension
    std::string filepath = path;
    size_t dotPos = filepath.find_last_of('.');
    std::string ext;
    if (dotPos != std::string::npos)
        ext = filepath.substr(dotPos);
    if (ext != ".obj")
    {
        printf("Unsupported mesh format: %s\n", ext.c_str());
        return {};
    }

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    // Map to remove duplicate vertices
    std::unordered_map<Vertex, uint16_t, VertexHash> vertex_map;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load OBJ file
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path))
    {
        printf("Failed to load obj file \"%s\" due to error: %s\n", path, err.c_str());
        return {};
    }
    if (!warn.empty())
        printf("Warning while loading obj file \"%s\" due to error: %s\n", path, warn.c_str());

    // Process all shapes
    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex =
            {
                .position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                },
                .color = {
                    uint8_t(attrib.colors[3 * index.vertex_index + 0] * 255),
                    uint8_t(attrib.colors[3 * index.vertex_index + 1] * 255),
                    uint8_t(attrib.colors[3 * index.vertex_index + 2] * 255),
                    255
                },
                .tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
                }
            };

            // Check if vertex already exists
            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end())
            {
                // Add new unique vertex
                auto new_vertex_index = static_cast<uint16_t>(vertices.size());
                vertex_map[vertex] = new_vertex_index;
                indices.push_back(new_vertex_index);
                vertices.push_back(vertex);
            }
            else
                // Reuse existing vertex
                indices.push_back(it->second);
        }
    }

    Mesh* mesh = new Mesh();
    mesh->create(vertices, indices);
    return mesh;
}

// Initialize static members
std::unordered_map<std::string, Mesh*> MeshLoader::mesh_cache;

// Memory management
void MeshLoader::unload(const std::string& filepath)
{
    auto cache_it = mesh_cache.find(filepath);
    if (cache_it != mesh_cache.end())
    {
        // Simply delete the Mesh object
        delete cache_it->second;
        mesh_cache.erase(cache_it);

        std::cout << "Unloaded mesh from cache: " << filepath << std::endl;
    }
    else
    {
        std::cout << "Mesh not found in cache: " << filepath << std::endl;
    }
}

void MeshLoader::unload_all()
{
    std::cout << "Unloading all cached meshes..." << std::endl;

    // Delete all Mesh objects
    for (auto& pair : mesh_cache)
    {
        delete pair.second;
    }

    // Clear the cache
    mesh_cache.clear();

    std::cout << "All cached meshes unloaded. Total: " << mesh_cache.size() << " meshes remaining." << std::endl;
}


bool MeshLoader::is_loaded(const std::string& filepath)
{
    return mesh_cache.find(filepath) != mesh_cache.end();
}

size_t MeshLoader::get_loaded_count()
{
    return mesh_cache.size();
}